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

### Example
```text
shell$ pwd
/home/abdellatif/CS/FCDS_Operating_Systems_Final_Project/Shell
shell$
```

### Testing (Valgrind)
```text
abdellatif@fedora:~/CS/FCDS_Operating_Systems_Final_Project/Shell$ valgrind --leak-check=full --show-leak-kinds=all ./myShell
==373094== Memcheck, a memory error detector
==373094== Copyright (C) 2002-2024, and GNU GPL'd, by Julian Seward et al.
==373094== Using Valgrind-3.26.0 and LibVEX; rerun with -h for copyright info
==373094== Command: ./myShell
==373094==
shell$ pwd
/home/abdellatif/CS/FCDS_Operating_Systems_Final_Project/Shell
shell$ pwd
/home/abdellatif/CS/FCDS_Operating_Systems_Final_Project/Shell
shell$
==373094==
==373094== HEAP SUMMARY:
==373094==     in use at exit: 0 bytes in 0 blocks
==373094==   total heap usage: 5 allocs, 5 frees, 1,112 bytes allocated
==373094==
==373094== All heap blocks were freed -- no leaks are possible
==373094==
==373094== For lists of detected and suppressed errors, rerun with: -s
==373094== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
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
$ valgrind --leak-check=full --show-leak-kinds=all ./myShell
==412301== Memcheck, a memory error detector
==412301== Command: ./myShell
==412301==
shell$ sleep 3 &
[Background PID 412310]
shell$ echo hello
hello
shell$
==412301== HEAP SUMMARY:
==412301==     in use at exit: 0 bytes in 0 blocks
==412301==   total heap usage: 12 allocs, 12 frees, 1,248 bytes allocated
==412301==
==412301== All heap blocks were freed -- no leaks are possible
==412301==
==412301== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

### Files changed

| File | Change |
|---|---|
| `myShell.c` | Added `<signal.h>`, zombie prevention, `&` detection, safe tokenisation, foreground/background wait logic, token `free()` |

No other files were modified.

---

## Project Progress

Based on all roadmap checkboxes, including subtasks, the project is at 54% completion (13/24).

Each checked box counts toward the total, not only the main roadmap items.

<progress value="13" max="24"></progress>

---

## Roadmap

- [x] Basic shell features
  - [x] Display a prompt such as `myShell$`. @abdellatif72
  - [x] Accept user input. @abdellatif72
  - [x] Parse commands and arguments. @abdellatif72
  - [x] Execute commands using `fork()`. @abdoheshamelsaid-bit
  - [x] Execute commands using the `exec()` family. @abdoheshamelsaid-bit
  - [x] Run external commands such as `ls`, `cat`, `wc`, `touch`, `mv`, `cp`, and `rm` through the normal execution path. @abdoheshamelsaid-bit

- [ ] Built-in commands
  - [x] `cd <directory>` @MohamedAlaa2005
  - [x] `exit` @ABDOMAGDY2005
  - [x] `pwd` @abdellatif72
  - [ ] `history` @khilo619

- [x] Process management
  - [x] Support foreground execution. @TLMostafa1650
  - [x] Support background execution using `&`. @TLMostafa1650
  - [x] Print the process ID for background processes. @TLMostafa1650

- [ ] Input/output redirection
  - [ ] Support output redirection with `>`.
  - [ ] Support input redirection with `<`.
  - [ ] Handle commands such as `ls > output.txt`.
  - [ ] Handle commands such as `cat < input.txt`.

- [x] Pipes
  - [x] Implement command piping using `|`. @Mariam7715y

- [ ] Signal handling
  - [ ] Handle `Ctrl+C` (`SIGINT`) without exiting the shell.
  - [ ] Terminate only the foreground child process when `Ctrl+C` is pressed.
  - [ ] Optionally support `Ctrl+Z` (`SIGTSTP`).

- [ ] Error handling
  - [ ] Handle command not found errors.
  - [ ] Handle file and directory errors.
  - [ ] Handle `fork()` and pipe failures.
