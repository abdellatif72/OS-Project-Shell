#include "Command.h"
#include <stdio.h>  // puts(), perror()
#include <string.h> // strdup()
#include <stdlib.h> // malloc(), free()
#include <unistd.h> // getcwd()

static void run(Command *self)
{
    char buf[1024]; // buffer for current working directory

    /*
        getcwd(buffer, size of buffer)
        if it fails, it returns NULL
        otherwise, it store the string of the current working directory in the buffer
    */
    if (getcwd(buf, sizeof(buf)) != NULL)
    {
        puts(buf);
    }
    else
    {
        perror("getcwd");
    }
}

static void help(Command *self)
{
    puts("pwd");
    puts("Print the name of the current working directory.");
}

/*
    destroy releases the resources owned by this command instance.
    free the fields before freeing the struct itself, because after
    free(self) the command pointer is no longer valid.
*/

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

Command *pwd_command()
{
    Command *cmd = (Command *)malloc(sizeof(Command));

    // since name in Command has no specific size, it is just a pointer that can point to any string, using it to point to "pwd", require allocating memory for that string using malloc, strdup() does that. No need to worry about what size to malloc and then copyting the text into it.
    cmd->name = strdup("pwd");
    cmd->args = NULL;
    cmd->run = run;
    cmd->help = help;
    cmd->destroy = destroy;

    return cmd;
}
