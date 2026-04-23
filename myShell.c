#include "Command.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h> 
#include <signal.h>    // signal handling (SIGCHLD)
#include <errno.h>

#define MAX_LINE 1024
#define MAX_BG 1000

Command *pwd_command();
Command *cd_command(char **args);
Command *history_command();
Command *exit_command();
void free_history();

void add_to_history(const char *cmd);
void get_input(char *buffer, int max_len);
void execute_pipeline(char **pipe_segments, int n_cmds);

/* ---------------- BACKGROUND PROCESS STORAGE ---------------- */
pid_t bg_pids[MAX_BG];
int bg_count = 0;

int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    signal(SIGINT, SIG_IGN); 

    while (1)
    {
        printf("\033[1;34mshell$ \033[0m");

        char user_input[MAX_LINE];
        get_input(user_input, MAX_LINE);

        if (user_input[0] == '\0')
            continue;

        add_to_history(user_input);

        /* ---------------- BACKGROUND DETECTION ---------------- */
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

        /* ---------------- PIPE HANDLING ---------------- */
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

        /* ---------------- SINGLE COMMANDS ---------------- */
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
            else if (strcmp(name, "history") == 0)
            {
                Command *cmd = history_command();
                cmd->run(cmd);
                cmd->destroy(cmd);
            }
            else if (strcmp(name, "exit") == 0)
            {
                Command *cmd = exit_command();
                cmd->run(cmd);
                cmd->destroy(cmd);

                for(int j = 0; j < token_count; j++){
                    free(tokens[j]);
                }

                free(name);

                free_history();

                return 0;
            }

            /* ---------------- EXTERNAL COMMANDS ---------------- */
            else
            {
                pid_t pid = fork();

                if (pid < 0)
                {
                    perror("fork failed");
                    free_history();
                }
                else if (pid == 0)
                {
                    signal(SIGINT, SIG_DFL);
                    execvp(tokens[0], tokens);
                    perror("exec failed");
                    free_history();
                    exit(1);
                }
                else
                {
                    if (background)
                    {
                        if (bg_count < MAX_BG)
                            bg_pids[bg_count++] = pid;

                        printf("[Background PID %d]\n", pid);
                    }
                    else
                    {
                        // wait for child to finish ( foreground )
                        int status = 0;
                        pid_t w;
                        do
                        {
                            w = waitpid(pid, &status, 0);
                        }
                        while (w == -1 && errno == EINTR);

                        if (w == pid && WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
                        {
                            printf("\n");
                        }
                    }
                    }
                }
                for (int j = 0; j < token_count; j++)
                    free(tokens[j]); // free strdup'd token copies

                free_history();
            }
        }

    return 0;
}

