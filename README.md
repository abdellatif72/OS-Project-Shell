# Shell in C

## How to run this program
1. Open the program folder. 
2. Open your bash terminal inside that folder
3. Type `make` and hit `enter`.

A new `shell_c' exectuable file will appear in the folder.

You can run it using your bash terminal

4. In your bash terminal. Type `./myShell` and hit `enter`

To clean your folder, you may use: `make clean` inside your bash terminal. It will remove the `myShell` and any output files.


## Project Progress

Based on all roadmap checkboxes, including subtasks, the project is at 0% completion (0/31).

Each checked box counts toward the total, not only the main roadmap items.

<progress value="0" max="31"></progress>


## Roadmap

- [ ] Basic shell features
	- [ ] Display a prompt such as `myShell$`.
	- [ ] Accept user input.
	- [ ] Parse commands and arguments.
	- [ ] Execute commands using `fork()`.
	- [ ] Execute commands using the `exec()` family.
	- [ ] Run external commands such as `ls`, `cat`, `wc`, `touch`, `mv`, `cp`, and `rm` through the normal execution path.

- [ ] Built-in commands
	- [ ] `cd <directory>`
	- [ ] `exit`
	- [ ] `pwd`
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
