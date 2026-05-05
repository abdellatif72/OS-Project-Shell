#include <stdio.h>     
#include <stdlib.h>     
#include <string.h>     
#include <unistd.h>     
#include <sys/wait.h>  

#define MAX_LINE 1024   // Maximum length of a command line

// Function to execute a pipeline of commands (e.g., ls | grep txt | wc -l)
void execute_pipeline(char **pipe_segments, int n_cmds) {

    // Array to store pipe file descriptors (each pipe has 2 ends: read/write)
    int pipefds[2 * (n_cmds - 1)];

    // Array to store process IDs of all child processes
    pid_t pids[n_cmds];

    // Create pipes between commands
    for (int i = 0; i < n_cmds - 1; i++) {

        // Create a pipe and store its read/write file descriptors
        if (pipe(pipefds + i * 2) < 0) {
            perror("pipe failed");  // Print error if pipe creation fails
            return;                  // Exit function
        }
    }

    // Loop over each command in the pipeline
    for (int i = 0; i < n_cmds; i++) {

        char *tokens[50];     // Array to store command + arguments
        int token_count = 0;  // Number of tokens found

        char segment_copy[MAX_LINE]; // Temporary buffer for command string

        // Copy command segment to avoid modifying original string
        strncpy(segment_copy, pipe_segments[i], MAX_LINE - 1);

        segment_copy[MAX_LINE - 1] = '\0'; // Ensure null termination

        // Split command into tokens (command + arguments)
        char *t = strtok(segment_copy, " \t");

        while (t != NULL && token_count < 49) {
            tokens[token_count++] = t;   // Store token
            t = strtok(NULL, " \t");     // Get next token
        }

        tokens[token_count] = NULL; // execvp requires NULL-terminated array

        // Skip empty commands if any
        if (token_count == 0)
            continue;

        // Create a new process for this command
        pids[i] = fork();

        // If fork failed
        if (pids[i] < 0) {
            perror("fork failed");
            return;
        }

        // Child process
        if (pids[i] == 0) {

            // If not the first command → take input from previous pipe
            if (i > 0) {
                if (dup2(pipefds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("dup2 stdin");
                    exit(1);
                }
            }

            // If not the last command → send output to next pipe
            if (i < n_cmds - 1) {
                if (dup2(pipefds[i * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2 stdout");
                    exit(1);
                }
            }

            // Close all pipe file descriptors in child (not needed anymore)
            for (int j = 0; j < 2 * (n_cmds - 1); j++) {
                close(pipefds[j]);
            }

            // Execute the command (replace process image)
            execvp(tokens[0], tokens);

            // If execvp fails (command not found)
            fprintf(stderr, "myShell: command not found: %s\n", tokens[0]);
            exit(127);
        }
    }

    // Parent process closes all pipe file descriptors
    for (int i = 0; i < 2 * (n_cmds - 1); i++) {
        close(pipefds[i]);
    }

    // Parent waits for all child processes to finish
    for (int i = 0; i < n_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }
}
