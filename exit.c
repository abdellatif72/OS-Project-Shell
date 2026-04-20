#include "Command.h"
#include <stdio.h>  // puts(), perror()
#include <string.h> // strdup()
#include <stdlib.h> // malloc(), free()
#include <unistd.h> // getcwd()

static void run(Command *self)
{

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
    {
        free(self->args);
    }

    free(self);
}

Command *exit_command()
{
    Command *cmd = (Command *)malloc(sizeof(Command));

    // since name in Command has no specific size, it is just a pointer that can point to any string, using it to point to "exit", require allocating memory for that string using malloc, strdup() does that. No need to worry about what size to malloc and then copyting the text into it.
    cmd->name = strdup("exit");
    cmd->args = NULL;
    cmd->run = run;
    cmd->help = help;
    cmd->destroy = destroy;

    return cmd;
}