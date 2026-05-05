# Linux Shell in C

## Table of Contents
- [Project Overview](#project-overview)
- [Objectives](#objectives)
- [Features Implemented](#features-implemented)
- [System Design](#system-design)
- [Project Structure](#project-structure)
- [Compilation & Execution](#compilation--execution)
- [Example Usage](#example-usage)
- [Contributors](#contributors)
- [Learning Outcomes](#learning-outcomes)
- [Conclusion](#conclusion)

## Project Overview

This project implements a **custom command-line shell** in C that mimics basic functionality of Unix/Linux shells.
The shell allows users to input commands, which are parsed and executed using low-level system calls.

The project focuses on understanding **process creation, command execution, pipes, background processing, and I/O redirection**.

---

## Objectives

- Understand how shells work internally
- Implement process creation using `fork()`
- Execute programs using `execvp()`
- Manage processes using `waitpid()`
- Support pipes, redirection, and background execution
- Implement built-in commands

---

## Features Implemented

### 1. Interactive Shell Loop

- Displays prompt: `shell$`
- Reads user input continuously

---

### 2. Command Parsing

- Splits input into:
  
  - commands (using `|`)
  - arguments (using spaces)
- Uses `strtok()` and `strdup()`
  

---

### 3. Built-in Commands

- `cd <directory>` → change directory (`chdir()`)
- `pwd` → print working directory (`getcwd()`)
- `history` → display previous commands
- `exit` → terminate shell and clean background processes

---

### 4. External Command Execution

- Uses:
  
  - `fork()` → create child process
  - `execvp()` → execute command
  - `waitpid()` → manage execution

---

### 5. Background Execution

- Supports `&`
- Does not block shell
- Stores process IDs
- Prevents zombies using `SIGCHLD`

---

### 6. Pipes (`|`)

- Supports command chaining:

```bash
ls | wc -l
```

- Uses `pipe()` and `dup2()`

---

### 7. Input/Output Redirection

- Supports input and output redirection:

```bash
ls > output.txt
cat < input.txt
echo "append" >> output.txt
```

- Uses `open()` and `dup2()`

---

### 8. Advanced Signal Handling

- Proper handling of `Ctrl+C (SIGINT)` without exiting the shell
- Terminates foreground process only

---

### 9. Error Handling

- Handles:
  
  - invalid commands / command not found
  - fork failures
  - pipe failures
  - directory errors

---

## System Design

### Execution Flow

```text
User Input
   ↓
Parse command
   ↓
Check built-in
   ↓
Handle Piping & Redirection
   ↓
fork()
   ↓
execvp()
   ↓
wait() / background
```

---

## Project Structure

```text
.
├── include/
│   ├── Command.h
│   └── redirection.h
├── src/
│   ├── myShell.c
│   ├── parser.c
│   ├── history.c
│   ├── cd.c
│   ├── pwd.c
│   ├── exit.c
│   ├── pipe_exec.c
│   └── redirection.c
├── Makefile
└── README.md
```

---

## Compilation & Execution

### Compile:

```bash
make
```

### Run:

```bash
./myShell
```

---

## Example Usage

```bash
shell$ pwd
/home/user/Shell
shell$ cd ..
shell$ ls
shell$ ls | wc -l
shell$ sleep 5 &
```

---

## Contributors

| Member | GitHub |
| --- | --- |
| Ahmed Abdellatif | [@abdellatif72](https://github.com/abdellatif72) |
| Abdo Hesham | [@abdoheshamelsaid-bit](https://github.com/abdoheshamelsaid-bit) |
| Abdo Magdy | [@ABDOMAGDY2005](https://github.com/ABDOMAGDY2005) |
| Khaled Nabil | [@khilo619](https://github.com/khilo619) |
| Mohamed Alaa | [@MohamedAlaa2005](https://github.com/MohamedAlaa2005) |
| Mostafa El-Hosseny | [@TLMostafa1650](https://github.com/TLMostafa1650) |
| Mariam Badry | [@Mariam7715y](https://github.com/Mariam7715y) |
| Michel Nader | [@micho789](https://github.com/micho789) |
| Mahmoud Shama3a | [@mahmoud2005mg-commits](https://github.com/mahmoud2005mg-commits) |
| Heba Ashraf | [@hebaandgithub](https://github.com/hebaandgithub) |

---

## Learning Outcomes

- Understanding process lifecycle
- Using system calls (`fork`, `exec`, `wait`)
- Implementing pipes and IPC
- Managing background processes
- Handling user input at low level

---

## Conclusion

This project demonstrates how a Unix shell works by combining parsing, process creation, execution, and inter-process communication.

---
