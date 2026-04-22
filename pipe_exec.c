#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 1024

void execute_pipeline(char **pipe_segments, int n_cmds) {
    int pipefds[2 * (n_cmds - 1)];
    pid_t pids[n_cmds];

    for (int i = 0; i < n_cmds - 1; i++) {
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe failed");
            return;
        }
    }

    for (int i = 0; i < n_cmds; i++) {
        char *tokens[50];
        int token_count = 0;

        char segment_copy[MAX_LINE];
        strncpy(segment_copy, pipe_segments[i], MAX_LINE - 1);
        segment_copy[MAX_LINE - 1] = '\0';

        char *t = strtok(segment_copy, " \t");
        while (t != NULL && token_count < 49) {
            tokens[token_count++] = t;
            t = strtok(NULL, " \t");
        }
        tokens[token_count] = NULL;

        if (token_count == 0)
            continue;

        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork failed");
            return;
        }

        if (pids[i] == 0) {
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("dup2 stdin");
                    exit(1);
                }
            }

            if (i < n_cmds - 1) {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2 stdout");
                    exit(1);
                }
            }

            for (int j = 0; j < 2 * (n_cmds - 1); j++) {
                close(pipefds[j]);
            }

            execvp(tokens[0], tokens);
            fprintf(stderr, "myShell: command not found: %s\n", tokens[0]);
            exit(127);
        }
    }

    for (int i = 0; i < 2 * (n_cmds - 1); i++) {
        close(pipefds[i]);
    }

    for (int i = 0; i < n_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }
}
