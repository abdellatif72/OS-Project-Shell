#include "Command.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* ---------------- SHARED FROM SHELL ---------------- */
extern pid_t bg_pids[];
extern int bg_count;

static void run(Command *self)
{
    for (int i = 0; i < bg_count; i++)
    {
        if (bg_pids[i] > 0)
        {
            kill(bg_pids[i], SIGTERM);
        }
    }
}


static void help(Command *self)
{
    puts("exit");
    puts("exit the shell.");
}

static void destroy(Command *self)
{
    if (self == NULL)
        return;

    free(self->name);

    if (self->args != NULL)
        free(self->args);

    free(self);
}

Command *exit_command()
{
    Command *cmd = malloc(sizeof(Command));

    cmd->name = strdup("exit");
    cmd->args = NULL;
    cmd->run = run;
    cmd->help = help;
    cmd->destroy = destroy;

    return cmd;
}