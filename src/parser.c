#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

// Interactive input reader with line editing and history support.
#define MAX_HIST 100

// Declarations of history globals defined in another source file (history.c). No storage here.
extern char *history_list[MAX_HIST];
extern int hist_count;

// Tracks the current input line state (buffer, length, cursor, history index).
typedef struct {
    char *buffer;
    int max_len;
    int pos;
    int cursor;
    int nav_idx;
} InputState;

// Save current terminal settings, then switch to raw mode (no ICANON/ECHO/ISIG).
static void set_raw_mode(struct termios *oldt);

// Restore the terminal settings that were saved before switching to raw mode.
static void restore_mode(const struct termios *oldt);

// Move the terminal cursor left by a given number of columns.
static void move_cursor_left(int count);

// Move the terminal cursor right by a given number of columns.
static void move_cursor_right(int count);

// Redraw the line from the current cursor to the end, then restore cursor position.
static void redraw_from_cursor(const InputState *state);

// Move cursor to column 0 and clear the current line before redrawing (e.g., history).
static void clear_line_and_reset_cursor(InputState *state);

// Delete the character before the cursor, shift the tail left, and redraw the line.
static void handle_backspace(InputState *state);

// Cancel the current line and reset input state (Ctrl+C behavior for the shell prompt).
static void handle_ctrl_c(InputState *state);

// Exit on Ctrl+D when the input line is empty.
static int handle_ctrl_d(const struct termios *oldt, const InputState *state);

// Load the selected history entry into the buffer and print it.
static void apply_history_entry(InputState *state);

// Handle Up/Down history navigation: clear line, update index, and render entry.
static void handle_history_nav(InputState *state, char dir);

// Handle arrow keys: history (Up/Down) and cursor movement (Left/Right).
static void handle_arrow_key(InputState *state, char key);

// Insert a character at the cursor, shift the tail right, and redraw the line.
static void handle_insert_char(InputState *state, char c);

// Read a line of input with editing/history support.
int get_input(char *buffer, int max_len);

// Save current terminal settings, then switch to raw mode (no ICANON/ECHO/ISIG).
static void set_raw_mode(struct termios *oldt)
{
    struct termios newt;
    // Read current terminal attributes and store them in oldt
    tcgetattr(STDIN_FILENO, oldt);

    // Make a modifiable copy of the current settings
    newt = *oldt;

    /*
     * Disable canonical mode (ICANON): read input byte-by-byte
     * Disable echo (ECHO): stop automatically printing typed characters
     * Disable signal generation (ISIG): let Ctrl+C be read as input so we can handle it
     * manually (useful because the parent ignores SIGINT).
     */
    newt.c_lflag &= ~(ICANON | ECHO | ISIG);

    // Apply the modified settings immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}


// Restore the terminal settings that were saved before switching to raw mode.
static void restore_mode(const struct termios *oldt)
{
    tcsetattr(STDIN_FILENO, TCSANOW, oldt);
}

// Move the cursor left by a given number of columns.
static void move_cursor_left(int count)
{
    for (int i = 0; i < count; i++) {
        printf("\033[D");
    }
}

// Move the cursor right by a given number of columns.
static void move_cursor_right(int count)
{
    for (int i = 0; i < count; i++) {
        printf("\033[C");
    }
}

// Redraw the line from the current cursor to the end, then restore cursor position.
// Used after edits like backspace to repaint the tail of the line.
static void redraw_from_cursor(const InputState *state)
{
    for (int i = state->cursor; i < state->pos; i++) {
        putchar(state->buffer[i]);
    }
    move_cursor_left(state->pos - state->cursor);
}

// Move cursor to column 0 and clear the current line before redrawing (e.g., history).
static void clear_line_and_reset_cursor(InputState *state)
{
    while (state->cursor > 0) {
        printf("\b");
        state->cursor--;
    }
    printf("\033[K");
}

// Delete the character before the cursor, shift the tail left, and redraw the line.
static void handle_backspace(InputState *state)
{
    if (state->cursor <= 0) {
        return;
    }

    for (int i = state->cursor - 1; i < state->pos - 1; i++) {
        state->buffer[i] = state->buffer[i + 1];
    }
    state->pos--;
    state->cursor--;

    printf("\b\033[K");
    redraw_from_cursor(state);
}

// Cancel the current line and reset input state (Ctrl+C behavior for the shell prompt).
static void handle_ctrl_c(InputState *state)
{
    printf("\n");
    printf("\033[1;34mshell$ \033[0m");
    state->pos = 0;
    state->cursor = 0;
    state->nav_idx = hist_count;
    memset(state->buffer, 0, state->max_len);
}

// Exit on Ctrl+D when the input line is empty.
static int handle_ctrl_d(const struct termios *oldt, const InputState *state)
{
    if (state->pos == 0) {
        restore_mode(oldt);
        printf("exit\n");
        return -1;
    }
    return 0;
}

// Load the selected history entry into the buffer and print it.
static void apply_history_entry(InputState *state)
{
    if (state->nav_idx < hist_count) {
        strncpy(state->buffer, history_list[state->nav_idx], state->max_len - 1);
        state->buffer[state->max_len - 1] = '\0';
    } else {
        state->buffer[0] = '\0';
    }

    state->pos = (int)strlen(state->buffer);
    state->cursor = state->pos;
    printf("%s", state->buffer);
}

// Handle Up/Down history navigation: clear line, update index, and render entry.
static void handle_history_nav(InputState *state, char dir)
{
    clear_line_and_reset_cursor(state);

    if (dir == 'A' && state->nav_idx > 0) {
        state->nav_idx--;
    } else if (dir == 'B' && state->nav_idx < hist_count) {
        state->nav_idx++;
    }

    apply_history_entry(state);
}

// Handle arrow keys: history (Up/Down) and cursor movement (Left/Right).
static void handle_arrow_key(InputState *state, char key)
{
    if (key == 'A' || key == 'B') {
        handle_history_nav(state, key);
        return;
    }

    if (key == 'C' && state->cursor < state->pos) {
        move_cursor_right(1);
        state->cursor++;
    } else if (key == 'D' && state->cursor > 0) {
        move_cursor_left(1);
        state->cursor--;
    }
}

// Insert a character at the cursor, shifting the tail to the right.
static void handle_insert_char(InputState *state, char c)
{
    if (state->pos >= state->max_len - 1) {
        return;
    }

    for (int i = state->pos; i > state->cursor; i--) {
        state->buffer[i] = state->buffer[i - 1];
    }
    state->buffer[state->cursor] = c;
    state->pos++;

    for (int i = state->cursor; i < state->pos; i++) {
        putchar(state->buffer[i]);
    }
    state->cursor++;
    move_cursor_left(state->pos - state->cursor);
}

// Read a line of input with in-line editing and history navigation.
int get_input(char *buffer, int max_len)
{
    struct termios oldt;

    // disable canonical input and echo and handle CTRL+C
    set_raw_mode(&oldt);

    InputState state = {
        .buffer = buffer,
        .max_len = max_len,
        .pos = 0,
        .cursor = 0,
        .nav_idx = hist_count
    };

    char c;
    while (1) {
        // read one byte at a time
        if (read(STDIN_FILENO, &c, 1) <= 0) {
            break;
        }

        if (c == '\n') { // ENTER key
            state.buffer[state.pos] = '\0';
            putchar('\n');
            break;
        } else if (c == 127 || c == 8) { // Backspace key
            handle_backspace(&state);
        } else if (c == 3) { // CTRL+C
            handle_ctrl_c(&state);
        } else if (c == 4) { // CTRL+D
            if (handle_ctrl_d(&oldt, &state) < 0) {
                return -1;
            }
        } else if (c == 27) { // Escape
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) <= 0) {
                continue;
            }
            if (read(STDIN_FILENO, &seq[1], 1) <= 0) {
                continue;
            }

            if (seq[0] == '[') {
                handle_arrow_key(&state, seq[1]);
            }
        } else { // Normal Character
            handle_insert_char(&state, c);
        }
    }

    restore_mode(&oldt);
    return 0;
}
