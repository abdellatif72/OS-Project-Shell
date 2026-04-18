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

## Project Progress

Based on all roadmap checkboxes, including subtasks, the project is at 10% completion (3/31).

Each checked box counts toward the total, not only the main roadmap items.

<progress value="3" max="31"></progress>

---

## Roadmap

- [ ] Basic shell features
  - [x] Display a prompt such as `myShell$`. @abdellatif72
  - [x] Accept user input. @abdellatif72
  - [x] Parse commands and arguments. @abdellatif72
  - [ ] Execute commands using `fork()`.
  - [ ] Execute commands using the `exec()` family.
  - [ ] Run external commands such as `ls`, `cat`, `wc`, `touch`, `mv`, `cp`, and `rm` through the normal execution path.

- [ ] Built-in commands
  - [ ] `cd <directory>`
  - [ ] `exit`
  - [x] `pwd` @abdellatif72
  - [ ] `history`

- [ ] Process management
  - [ ] Support foreground execution.
  - [ ] Support background execution using `&`.
  - [ ] Print the process ID for background processes.

- [ ] Input/output redirection
  - [ ] Support output redirection with `>`.
  - [ ] Support input redirection with `<`.
  - [ ] Handle commands such as `ls > output.txt`.
  - [ ] Handle commands such as `cat < input.txt`.

- [ ] Pipes
  - [ ] Implement command piping using `|`.

- [ ] Signal handling
  - [ ] Handle `Ctrl+C` (`SIGINT`) without exiting the shell.
  - [ ] Terminate only the foreground child process when `Ctrl+C` is pressed.
  - [ ] Optionally support `Ctrl+Z` (`SIGTSTP`).

- [ ] Error handling
  - [ ] Handle command not found errors.
  - [ ] Handle file and directory errors.
  - [ ] Handle `fork()` and pipe failures.
