#include "Command.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

#define MAX_HIST 100
#define MAX_LEN 1024

static char *history_list[MAX_HIST];
static int hist_count = 0;
static int curr_idx = -1;

void add_to_history(const char *cmd) {
    if (cmd == NULL || strlen(cmd) == 0) {
        return;
    }

    if (hist_count > 0 && strcmp(history_list[hist_count - 1], cmd) == 0) {
        return;
    }

    if (hist_count < MAX_HIST) {
        history_list[hist_count++] = strdup(cmd);
    } else {
        free(history_list[0]);
        for (int j = 1; j < MAX_HIST; j++) {
            history_list[j - 1] = history_list[j];
        }
        history_list[MAX_HIST - 1] = strdup(cmd);
    }

    curr_idx = hist_count;
}

void run_history(Command *self) {
    for (int i = 0; i < hist_count; i++) {
        printf("%d %s\n", i+1, history_list[i]);
    }
}

void destroy_history(Command *self) {
    if (self) {
        free(self);
    }
}

Command *history_command() {
    Command *cmd = calloc(1, sizeof(Command));
    if (!cmd) {
        return NULL;
    }
    cmd -> name = "history";
    cmd -> run = run_history;
    cmd -> destroy = destroy_history;
    return cmd;
}

void get_input(char *buffer, int max_len) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    int pos = 0;
    int cursor = 0;
    int nav_idx = hist_count;
    char c;
    while(1) {
        if (read(STDIN_FILENO, &c, 1) <= 0) {
            break;
        }

        if (c == '\n') {
            buffer[pos] = '\0';
            putchar('\n');
            break;
        } else if (c ==  127 || c == 8) {
            if (pos > 0) {
                pos--;
                printf("\b \b");
            }
        } else if (c == 3) {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            printf("^C\n");
            buffer[0] = '\0';
            break;
        } else if (c == 4) {
            if (pos == 0) {
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                printf("exit\n");
                exit(0);
            }
        } else if (c == 27) {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 0) {
                break;
            }
            if (read(STDIN_FILENO, &seq[1], 1) == 0) {
                break;
            }

            if (seq[0] =='[') {
                if (seq[1] == 'A') {
                    if (nav_idx > 0) {
                        nav_idx--;
                        while (cursor > 0) {
                            printf("\033[D");
                            cursor--;
                        }
                        printf("\033[K");
                        strncpy(buffer, history_list[nav_idx], max_len - 1);
                        pos = strlen(buffer);
                        cursor = pos;
                        printf("%s", buffer);
                    }
                } else if (seq[1] == 'B') {
                    if (nav_idx < hist_count - 1) {
                        nav_idx++;
                        while(cursor > 0) { 
                            printf("\033[D");
                            cursor--; 
                        }
                        printf("\033[K");
                        if (nav_idx < hist_count) {
                            strncpy(buffer, history_list[nav_idx], max_len - 1);
                        } else {
                            buffer[0] = '\0';
                        }
                        pos = strlen(buffer);
                        cursor = pos;
                        printf("%s", buffer);
                    }
                } else if (seq[1] == 'C') {
                    if (cursor < pos) {
                        printf("\033[C");
                        cursor++;
                    }
                } else if (seq[1] == 'D') {
                    if (cursor > 0) {
                        printf("\033[D");
                        cursor--;
                    }
                }   
            }
        } else {
            if (pos < max_len - 1) {
                for (int i = pos; i > cursor; i--) {
                    buffer[i] = buffer[i - 1];
                }
                buffer[cursor] = c;
                pos++;

                for (int i = cursor; i < pos; i++) {
                    putchar(buffer[i]);
                }
                cursor++;

                for (int i = 0; i < (pos - cursor); i++) {
                    printf("\033[D"); 
                }
            }
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}
 
