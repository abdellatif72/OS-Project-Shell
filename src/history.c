#include "Command.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

// Command history storage and built-in implementation.
#define MAX_HIST 100
#define MAX_LEN 1024

// History storage (global so parser/input can access it).
char *history_list[MAX_HIST];
int hist_count = 0;
// Current navigation index used while browsing history.
static int curr_idx = -1;

// Add a command line to history with duplicate/size handling.
void add_to_history(const char *cmd)
{
    // Ignore empty input
    if (cmd == NULL || strlen(cmd) == 0)
    {
        return;
    }

    // Avoid adding the same command twice in a row
    if (hist_count > 0 && strcmp(history_list[hist_count - 1], cmd) == 0)
    {
        return;
    }

    // Append if there is space; otherwise, drop the oldest entry and shift left
    if (hist_count < MAX_HIST)
    {
        history_list[hist_count++] = strdup(cmd);
    }
    else
    {
        free(history_list[0]);
        for (int j = 1; j < MAX_HIST; j++)
        {
            history_list[j - 1] = history_list[j];
        }
        history_list[MAX_HIST - 1] = strdup(cmd);
    }

    // Reset navigation index to the end of the history list
    curr_idx = hist_count;
}

// Print the full history list.
void run_history(Command *self)
{
    for (int i = 0; i < hist_count; i++)
    {
        printf("%d %s\n", i + 1, history_list[i]);
    }
}

// Free the Command wrapper for the history built-in.
void destroy_history(Command *self)
{
    if (self)
    {
        free(self);
    }
}

// Construct a Command object for the history built-in.
Command *history_command()
{
    Command *cmd = malloc(sizeof(Command));
    if (!cmd)
    {
        return NULL;
    }
    cmd->name = "history";
    cmd->run = run_history;
    cmd->destroy = destroy_history;
    return cmd;
}



// Free all stored history entries.
void free_history()
{
    for (int i = 0; i < hist_count; i++)
    {
        free(history_list[i]);
        history_list[i] = NULL;
    }

    hist_count = 0;
}
