#include "redirection.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void handle_redirection(char **tokens)
{
    for (int i = 0; tokens[i] != NULL; i++)
    {
        // Output redirection >
        if (strcmp(tokens[i], ">") == 0)
        {
            if (tokens[i + 1] == NULL)
            {
                fprintf(stderr, "Syntax error: no output file\n");
                exit(1);
            }

            int fd = open(tokens[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0)
            {
                perror("open failed");
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);

            tokens[i] = NULL; // cut command here
        }

        // Input redirection <
        else if (strcmp(tokens[i], "<") == 0)
        {
            if (tokens[i + 1] == NULL)
            {
                fprintf(stderr, "Syntax error: no input file\n");
                exit(1);
            }

            int fd = open(tokens[i + 1], O_RDONLY);
            if (fd < 0)
            {
                perror("open failed");
                exit(1);
            }

            dup2(fd, STDIN_FILENO);
            close(fd);

            tokens[i] = NULL;
        }

        // Append output redirection >>
        else if (strcmp(tokens[i], ">>") == 0)
        {
            if (tokens[i + 1] == NULL)
            {
                fprintf(stderr, "Syntax error: no output file\n");
                exit(1);
            }

            int fd = open(tokens[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0)
            {
                perror("open failed");
                exit(1);
            }

            dup2(fd, STDOUT_FILENO);
            close(fd);

            tokens[i] = NULL;
        }
    }
}