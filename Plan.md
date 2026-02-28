# Operating Systems - Unix Shell

## 1. Problem Analysis

The goal is to implement a simplified Unix shell, `mysh`, that supports:
- Interactive command execution with its own prompt
- Built-in commands (`cd`, `pwd`, `exit`)
- External command execution (via `fork()` and `exec()`)
- Input/output redirection (`>`, `>>`, `<`)
- Background process management (`&`)
- Robust error handling and memory management

Challenges include safely parsing user input, distinguishing between built-in and external commands, handling process creation and termination, and managing file descriptors for I/O redirection.

## 2. Solution Architecture

### a. High-Level Design Overview

The shell will follow a classic Read-Eval-Print Loop (REPL):

1. **Read:** Print prompt and read a line of input from the user.
2. **Parse:** Tokenize the input to identify the command, arguments, I/O redirection, and background execution operators (`&`).
3. **Evaluate:**
   - If built-in (`cd`, `pwd`, `exit`): Execute directly in the shell process.
   - Else: Fork a child, set up any necessary I/O redirection, and execute the external command.
   - If background mode: Parent does not wait, else `waitpid()` for child.
   - Track background jobs and handle child termination (to prevent zombies).
4. **Print:** Output command results, errors, or job information.

### b. Main Modules and Key Functions

- **main.c:** Shell entry point and REPL loop
- **parser.c/h:** Input parsing and command structure filling
- **executor.c/h:** Execution logic (distinguish built-in vs. external, process creation, background handling)
- **builtins.c/h:** Implementation of `cd`, `pwd`, and `exit`
- **jobs.c/h:** Background jobs tracking, cleanup, and status reporting
- **utils.c/h:** Helper functions (error messages, safe memory handling, etc.)

### c. Data Structures

A `Command` struct (as suggested) will encapsulate:
- Command name and argv array
- Redirection filenames (input/output) and append/truncate
- Background execution flag

A job list (array or linked list) will track running background processes.

### d. Error Handling

All system calls (e.g., `fork`, `execvp`, `open`, `chdir`) will check return values with appropriate error messages (using `perror`). Edge cases (missing files, malformed input, failed execution) will be handled gracefully.

### e. Modularity and Code Quality

- Functions will be small and focused.
- Header files will be used for interfaces.
- Code will be documented and follow consistent conventions.

---

## 3. Estimated Implementation Timeline

**Week 1:**  
- Set up repository, read assignment and clarify requirements  

**Week 2:** 
- Implement basic REPL loop and parsing
- Add simple execution of external commands 
- Add built-in commands (`cd`, `pwd`, `exit`)
- Implement I/O redirection (`<`, `>`, `>>`)
- Begin implementing background job control (`&`, job tracking)

**Week 3:**  
- Complete background job subsystem and cleanup logic
- Thorough error handling, memory leak checks
- Modularize code into multiple files
- Test for all required (and edge) cases

**Week 4:**  
- Prepare documentation (README.md, reflection.txt, peer.txt)
- Prepare test scripts
- Final polish, team code review
- Submit and prepare for defense

---

## 4. Additional Notes

- Compare feature behavior with bash for correctness.
- Test all code paths (invalid input, permission errors, etc.).
- Use git for collaborative, incremental development (`feature-branch` workflow).
- Address all requirements in the rubric.


*Prepared by: Andrea Laserna and Cedric Oyco*