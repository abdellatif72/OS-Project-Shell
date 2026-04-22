#include "Command.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <signal.h>    // signal handling (SIGCHLD)

#define MAX_LINE 1024

Command *pwd_command();
Command *cd_command(char **args);
void execute_pipeline(char **pipe_segments, int n_cmds); 

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    signal(SIGCHLD, SIG_IGN);      /*prevents zombie processes for background execution
                                     When a child finishes, OS cleans it automatically */

    

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

         // detect if command should run in background using '&'
        int background = 0;
        int len = strlen(user_input);

        if (len > 0 && user_input[len - 1] == '&')
        {
            background = 1;              
            user_input[len - 1] = '\0';  // remove '&' so execvp doesn't see it

            // remove trailing spaces after '&'

            while (len > 1 && user_input[len - 2] == ' ')
            {
                user_input[len - 2] = '\0';
                len--;
            }
        }



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

        if (argc > 1) {
            execute_pipeline(commands, argc);
            continue;  // done handling this user_input line
        }

        /* existing single-command logic here (your current for-loop body, simplified to one command) */
        for (int i = 0; i < argc; i++)
        {
            /* split by spaces */
            char *tokens[50];
            int token_count = 0;

            char segment_copy[MAX_LINE];
            strncpy(segment_copy, commands[i], MAX_LINE - 1);
            segment_copy[MAX_LINE - 1] = '\0';

            char *t = strtok(segment_copy, " \t");
            while (t != NULL && token_count < 49)
            {
                tokens[token_count++] = strdup(t);   // strdup keeps a safe copy
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

                      if (pid < 0)
                {
                    perror("fork failed");
                }

                else if (pid == 0)
                {
                    execvp(tokens[0], tokens);
                    perror("exec failed");
                    exit(1);
                }
                else
                {
                    if (background)
                    {
                        /* run in background → Don't Wait
                           print PID so user knows process ID */

                        printf("[Background PID %d]\n", pid);
                    }
                    else
                    {
                        // wait for child to finish ( foreground )
                        waitpid(pid, NULL, 0);
                    }
                }
            }
            for (int j = 0; j < token_count; j++)
                free(tokens[j]); // free strdup'd token copies
        }
    }

    return 0;
}

