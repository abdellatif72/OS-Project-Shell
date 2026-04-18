// to prevent multiple inclusion of this header file.
#pragma once 

// a simulation of an abstract class that will be used by all commands
typedef struct Command
{
    char* name;
    char** args;


    // a simulation of abstract functions that will be overriden in other commands
    void (*run)(struct Command* self);
    void (*help)(struct Command* self);
    void (*destroy)(struct Command* self);
} Command;
