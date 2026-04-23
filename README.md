# Shell in C

## How to build and run

### Requirements
- Linux environment
- `gcc`
- `make`

### Build
1. Open a terminal in the project directory (`Shell/`).
2. Run:
   `make`

This generates the executable: `myShell`.

### Run
Start the shell with:
`./myShell`

### Exit
You can exit by:
- typing `exit` (if implemented), or
- pressing `Ctrl + D` (EOF).

### Clean build files
To remove generated files, run:
`make clean`

This removes:
- `myShell`
- any object/output files produced by the build process.

---

## `pwd`

### Purpose
Print the absolute path of the current working directory.

### Syntax
`pwd`

### Arguments
This command takes no arguments.

### Behavior
- On success, prints the current directory path.
- On failure, prints an error message.

### Implementation details
This command uses `getcwd()` from `<unistd.h>`.

`getcwd(buffer, size)`:
- returns `buffer` (a pointer to the resulting string) on success.
- returns `NULL` on failure.

In this project, `pwd` is implemented in `pwd.c` inside `run()`, which:
1. allocates a local buffer,
2. calls `getcwd(...)`,
3. prints the path on success,
4. otherwise calls `perror("getcwd")`.

### Examples
```text
shell$ pwd
/home/abdellatif/CS/FCDS_Operating_Systems_Final_Project/Shell
shell$
```

```text
abdellatif@fedora:~/CS/FCDS_Operating_Systems_Final_Project/shell_1$ ./myShell 
shell$ ls | wc -l
10
shell$ exit
```

### Testing (Valgrind)
```text
abdellatif@fedora:~/CS/FCDS_Operating_Systems_Final_Project/shell_1$ valgrind --leak-check=full --show-leak-kinds=all ./myShell 
==267522== Memcheck, a memory error detector
==267522== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==267522== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
==267522== Command: ./myShell
==267522== 
shell$ pwd
/home/abdellatif/CS/FCDS_Operating_Systems_Final_Project/shell_1
shell$ cd ..
shell$ cd shell_1
shell$ ls
cd.c  Command.h  exit.c  history.c  Makefile  myShell  myShell.c  pipe_exec.c  pwd.c  README.md
shell$ echo "wow"
"wow"
shell$ cat exit.c
#include "Command.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* ---------------- SHARED FROM SHELL ---------------- */
extern pid_t bg_pids[];
extern int bg_count;

static void run(Command *self)
{
    for (int i = 0; i < bg_count; i++)
    {
        if (bg_pids[i] > 0)
        {
            kill(bg_pids[i], SIGTERM);
        }
    }
}


static void help(Command *self)
{
    puts("exit");
    puts("exit the shell.");
}

static void destroy(Command *self)
{
    if (self == NULL)
        return;

    free(self->name);

    if (self->args != NULL)
        free(self->args);

    free(self);
}

Command *exit_command()
{
    Command *cmd = malloc(sizeof(Command));

    cmd->name = strdup("exit");
    cmd->args = NULL;
    cmd->run = run;
    cmd->help = help;
    cmd->destroy = destroy;

    return cmd;
}shell$ sleep 5
^C
shell$ sleep 5 &
[Background PID 267954]
shell$ exit
==267522== 
==267522== HEAP SUMMARY:
==267522==     in use at exit: 0 bytes in 0 blocks
==267522==   total heap usage: 34 allocs, 34 frees, 321 bytes allocated
==267522== 
==267522== All heap blocks were freed -- no leaks are possible
==267522== 
==267522== For lists of detected and suppressed errors, rerun with: -s
==267522== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```


---

## Background Execution

### Purpose
Run a command in the background so the shell remains usable while the command executes concurrently.

### Syntax
`<command> [arguments] &`

### Arguments
The `&` symbol must be the last character of the input line. It is not passed to the command itself.

### Behavior
- If `&` is present, the shell prints the child process ID and returns to the prompt immediately.
- If `&` is absent, the shell waits for the command to finish before showing the prompt again (foreground execution).
- Finished background processes are cleaned up automatically — no zombie processes are left behind.

### Implementation details
All changes are in `myShell.c`.

**`signal(SIGCHLD, SIG_IGN)`**

Called once before the main loop. When a background child finishes, the OS normally keeps its exit record until the parent calls `wait()`. Setting `SIGCHLD` to `SIG_IGN` tells the kernel to discard those records automatically, preventing zombie processes.

**`&` detection**

After reading input and stripping the newline, the shell checks whether the last character is `&`. If so, `background` is set to `1`, the `&` is removed from the string, and any trailing spaces are trimmed so `execvp()` receives a clean command.

**Safe tokenisation with `strdup()`**

The original code called `strtok()` twice on the same string — once to split by `|` and again to split by spaces. Because `strtok()` uses a single internal pointer, the second call reset it and arguments were lost. The fix copies each pipe segment into a local buffer before tokenising it. Each token is then duplicated with `strdup()` so it owns its own memory.

**Foreground vs background wait**

In the parent branch after `fork()`:
- `background == 0` → `waitpid()` blocks until the child exits.
- `background == 1` → `waitpid()` is skipped; the PID is printed and the loop continues.

**Token cleanup**

Each token was heap-allocated with `strdup()`, so `free()` is called on every token at the end of each loop iteration, preventing memory leaks even when `fork()` fails.

### Example
```text
shell$ sleep 5 &
[Background PID 1042]
shell$
shell$ sleep 3
shell$
```

### Testing (Valgrind)
```text
abdellatif@fedora:~/CS/FCDS_Operating_Systems_Final_Project/shell_1$ valgrind --leak-check=full --show-leak-kinds=all ./myShell 
==1261720== Memcheck, a memory error detector
==1261720== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==1261720== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
==1261720== Command: ./myShell
==1261720== 
shell$ ls
cd.c  Command.h  exit.c  history.c  Makefile  myShell  myShell.c  omg.txt  pipe_exec.c  pwd.c  README.md
shell$ cd ..
shell$ cd shell_1
shell$ ls
cd.c  Command.h  exit.c  history.c  Makefile  myShell  myShell.c  omg.txt  pipe_exec.c  pwd.c  README.md
shell$ echo "wow" > text.txt
shell$ ls
cd.c  Command.h  exit.c  history.c  Makefile  myShell  myShell.c  omg.txt  pipe_exec.c  pwd.c  README.md  text.txt
shell$ cat text.txt
"wow"
shell$ ls | wc -l
12
shell$ ls
cd.c  Command.h  exit.c  history.c  Makefile  myShell  myShell.c  omg.txt  pipe_exec.c  pwd.c  README.md  text.txt
shell$ sleep 5 &
[Background PID 1262312]
shell$ sleep 6
^C
shell$ exit
==1261720== 
==1261720== HEAP SUMMARY:
==1261720==     in use at exit: 0 bytes in 0 blocks
==1261720==   total heap usage: 44 allocs, 44 frees, 357 bytes allocated
==1261720== 
==1261720== All heap blocks were freed -- no leaks are possible
==1261720== 
==1261720== For lists of detected and suppressed errors, rerun with: -s
==1261720== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### Files changed

| File | Change |
|---|---|
| `myShell.c` | Added `<signal.h>`, zombie prevention, `&` detection, safe tokenisation, foreground/background wait logic, token `free()` |

No other files were modified.

---

## Project Progress

Based on the roadmap subtasks, the project is at 70% completion (19/27).

Only the specific subtasks count toward the total, excluding the main category headers.

<progress value="19" max="27"></progress>

---

## Roadmap

- [x] Basic shell features
  - [x] Display a prompt such as `myShell$`. @abdellatif72
  - [x] Accept user input. @khilo619 & @abdellatif72
  - [x] Parse commands and arguments. @khilo619 & @abdellatif72 
  - [x] Execute commands using `fork()`. @abdoheshamelsaid-bit
  - [x] Execute commands using the `exec()` family. @abdoheshamelsaid-bit
  - [x] Run external commands such as `ls`, `cat`, `wc`, `touch`, `mv`, `cp`, and `rm` through the normal execution path. @abdoheshamelsaid-bit

- [x] Built-in commands
  - [x] `cd <directory>` @MohamedAlaa2005
  - [x] `exit` @ABDOMAGDY2005
  - [x] `pwd` @abdellatif72
  - [x] `history` @khilo619

- [x] Process management
  - [x] Support foreground execution. @TLMostafa1650
  - [x] Support background execution using `&`. @TLMostafa1650
  - [x] Print the process ID for background processes. @TLMostafa1650
  - [x] Store the process ID for background processes. @ABDOMAGDY2005
  - [x] Kill the background processes when exiting the shell. @ABDOMAGDY2005
  - [x] Handle freeing the history of commands. @abdellatif72

- [x] Input/output redirection @micho789
  - [x] Handle commands such as `ls > output.txt`. @micho789
  - [x] Handle commands such as `cat < input.txt`. @micho789

- [x] Pipes
  - [x] Implement command piping using `|`. @Mariam7715y

- [x] Signal handling
  - [x] Handle `Ctrl+C` (`SIGINT`) without exiting the shell. @khilo619
  - [x] Terminate only the foreground child process when `Ctrl+C` is pressed. @khilo619

- [x] Error handling
  - [x] Handle command not found errors.
  - [x] Handle file and directory errors.
  - [x] Handle `fork()` and pipe failures.

---

## Contributors

- [@abdellatif72](https://github.com/abdellatif72)
- [@abdoheshamelsaid-bit](https://github.com/abdoheshamelsaid-bit)
- [@MohamedAlaa2005](https://github.com/MohamedAlaa2005)
- [@ABDOMAGDY2005](https://github.com/ABDOMAGDY2005)
- [@khilo619](https://github.com/khilo619)
- [@TLMostafa1650](https://github.com/TLMostafa1650)
- [@Mariam7715y](https://github.com/Mariam7715y)
- [@micho789](https://github.com/micho789)
