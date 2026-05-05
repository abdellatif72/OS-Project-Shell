#include "Command.h"
#include <stdio.h>   // perror()
#include <string.h>  // strdup()
#include <stdlib.h>  // malloc(), free()
#include <unistd.h>  // chdir()

// Built-in "cd" command implementation.

// Execute the cd command using the provided arguments.
static void run(Command *self)
{
    /*
        self->args[0] is the command name ("cd")
        self->args[1] is the target directory path
        if no argument is given, we do nothing (or could default to HOME)
    */
    if (self->args == NULL || self->args[1] == NULL)
    {
        fprintf(stderr, "cd: missing argument\n");
        return;
    }

    /*
        chdir(path) changes the current working directory of the process.
        returns 0 on success, -1 on failure (e.g. path does not exist)
    */
    if (chdir(self->args[1]) != 0)
    {
        perror("cd");
    }
}

// Print usage information for cd.
static void help(Command *self)
{
    puts("cd <directory>");
    puts("Change the current working directory to <directory>.");
}

// Free resources owned by this cd command instance.
static void destroy(Command *self)
{
    if (self == NULL)
        return;

    free(self->name);

    /*
        args[0] is the command name, args[1..] are the actual arguments.
        we free each string individually since each was strdup'd,
        then free the array itself.
    */
    if (self->args != NULL)
    {
        for (int i = 0; self->args[i] != NULL; i++)
        {
            free(self->args[i]);
        }
        free(self->args);
    }

    free(self);
}

// Construct a Command object for the cd built-in.
Command *cd_command(char **args)
{
    Command *cmd = (Command *)malloc(sizeof(Command));

    cmd->name = strdup("cd");

    /*
        count how many args were passed so we know how large to make the array.
        args is expected to be a NULL-terminated array of strings, e.g.:
        { "cd", "/home/user", NULL }
    */
    int count = 0;
    if (args != NULL)
    {
        while (args[count] != NULL)
            count++;
    }

    /*
        allocate (count + 1) slots: one per argument plus a NULL terminator.
        then strdup each string so this Command owns its own copies.
    */
    cmd->args = (char **)malloc((count + 1) * sizeof(char *));
    for (int i = 0; i < count; i++)
    {
        cmd->args[i] = strdup(args[i]);
    }
    cmd->args[count] = NULL; // NULL-terminate the args array

    cmd->run = run;
    cmd->help = help;
    cmd->destroy = destroy;

    return cmd;
}