#include "Command.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_LINE 1024

Command *pwd_command();
Command *cd_command(char **args);

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    while (1)
    {
        printf("\033[1;34mshell$ \033[0m");

        char user_input[MAX_LINE];

        if (fgets(user_input, sizeof(user_input), stdin) == NULL)
        {
            if (feof(stdin))
            {
                clearerr(stdin);
                putchar('\n');
                continue;
            }
            break;
        }

        user_input[strcspn(user_input, "\n")] = '\0';

        if (user_input[0] == '\0')
            continue;

        /* split by pipe */
        char *commands[50];
        int argc = 0;

        char *tok = strtok(user_input, "|");

        while (tok != NULL && argc < 49)
        {
            commands[argc++] = tok;
            tok = strtok(NULL, "|");
        }

        commands[argc] = NULL;

        for (int i = 0; i < argc; i++)
        {
            /* split by spaces */
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

            /* ---------------- BUILT-IN COMMANDS ---------------- */

            if (strcmp(name, "pwd") == 0)
            {
                Command *cmd = pwd_command();
                cmd->run(cmd);
                cmd->destroy(cmd);
            }
            else if (strcmp(name, "cd") == 0)
            {
                Command *cmd = cd_command(tokens);
                cmd->run(cmd);
                cmd->destroy(cmd);
            }
            else if (strcmp(name, "exit") == 0)
            {
                return 0;
            }

            /* ---------------- EXTERNAL COMMANDS ---------------- */
            else
            {
                pid_t pid = fork();

                if (pid == 0)
                {
                    execvp(tokens[0], tokens);
                    perror("exec failed");
                    exit(1);
                }
                else
                {
                    wait(NULL);
                }
            }
        }
    }

    return 0;
}

