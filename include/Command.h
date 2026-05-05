// Prevent multiple inclusion of this header file.
#pragma once

// Command interface used by built-in commands.
typedef struct Command
{
    // Command name (e.g., "cd", "pwd").
    char *name;
    // NULL-terminated argument list owned by the command.
    char **args;

    // Execute the command.
    void (*run)(struct Command *self);
    // Print command usage/help.
    void (*help)(struct Command *self);
    // Release resources owned by this command.
    void (*destroy)(struct Command *self);
} Command;
