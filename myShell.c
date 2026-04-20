#include "Command.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_LINE 1024

Command *pwd_command();
Command *cd_command(char **args); // declare cd_command

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    while (1)
    {
        // to print "shell$" in blue
        printf("\033[1;34mshell$ \033[0m");

        char user_input[MAX_LINE];

        if (fgets(user_input, sizeof(user_input), stdin) == NULL)
        {
            // in case that the user inputs: CTRL+D (EOF)
            if (feof(stdin))
            {
                clearerr(stdin);
                putchar('\n');
                continue; // skip and just print a new line to read new input: shell$
            }

            break;
        }

        user_input[strcspn(user_input, "\n")] = '\0';
        if (user_input[0] == '\0')
            continue;

        char *commands[50];
        int argc = 0;
        char *tok = strtok(user_input, "|");
        while (tok != NULL && argc < 49)
        {
            commands[argc++] = tok;
            tok = strtok(NULL, "|");
        }
        commands[argc] = NULL;

        for (int i = 0; i < argc && commands[i] != NULL; i++)
        {
            /*
                tokenize this sub-command by spaces/tabs to extract
                the command name and its arguments into a small array.
            */
            char *tokens[50];
            int token_count = 0;

            char *t = strtok(commands[i], " \t");
            while (t != NULL && token_count < 49)
            {
                tokens[token_count++] = t;
                t = strtok(NULL, " \t");
            }
            tokens[token_count] = NULL;

            if (token_count == 0)
                continue;

            char *name = tokens[0];

            if (strcmp(name, "pwd") == 0)
            {
                Command *cmd = pwd_command();
                cmd->run(cmd);
                cmd->destroy(cmd);
            }
            else if (strcmp(name, "cd") == 0)
            {
                /*
                    pass the full tokens array (starting with "cd") to cd_command
                    so args[0]="cd", args[1]=path, args[2]=NULL
                */
                Command *cmd = cd_command(tokens);
                cmd->run(cmd);
                cmd->destroy(cmd);
            }
            else if (strcmp(name, "exit") == 0)
            {
                return 0;
            }
        }
    }

    return 0;
}
