
let getline allocate buffer, command, etc. since we don't know how long/big the input is anyway

## main variables
command = buffer holding raw line read from stdin
cmd_copy = duplicate of command used for token counting, since strtok modifies strings
token = current token returned by strtok while parsing
delimiter = token separators
argc = no of tokens (arguments) found
i = index to fill argv
argv = array of char* pointers holding arguments for execvp; null-terminated
pid = processid returned by fork to differentiate parent from child


============================================

## Design Decisions: 
1. why use getline?
	- It is a convenient way to read an entire line of input, regardless of its length.
	- It automatically allocates a buffer and resizes it as needed, which simplifies memory management.
	- It handles the newline character at the end of the input, which can be useful for processing commands.
	* why use EXIT_SUCCESS and EXIT_FAILURE?
		- for future automated testing, it is easier to check for these standard exit codes rather than arbitrary integers.
2. why use strtok?
	- It is a simple and efficient way to split a string into tokens based on specified delimiters.
	- However, it modifies the original string by inserting null characters to terminate tokens
	- Try to implement a version that does not modify the original string maybe? 

3. why use strdup?
	- It is a convenient way to create a copy of a string, which is necessary because strtok modifies the original string.
	- It allocates memory for the new string and copies the contents, so we need to remember to free it later to avoid memory leaks.
	- However, this current approach seems inefficient since we are making a copy of the command just to tokenize it, 
		and then we are tokenizing it again to fill the argv array. 
		|
		argv = realloc(argv, argc * sizeof(char *)); // Resize argv to hold the new token
        argv[argc - 1] = strdup(token); // Store a copy of the token in argv
		|
		this is one way to avoid the copy but it leads to multiple allocations so we need to free each token in argv, kapoy na ya.
		think of a better way to implement this. 
		try strtok_r 
4. Loop implementaion
	- currently utilizes infinite loop. It initializes argc and i at the start and frees allocated memory (command,cmd_copy, argv) after execution. 
	- program terminate with ctrl + z or something, 
	- to implement exit command


============================================

## Built-in: exit (no forking)
* target any child of the shell
* ignore child's exit status
* WNOHANG
* waitpid does not wait; return immediately to prevent pausing shell:
    - >0: child pid reaped (finished child collected)
    - 0:  no child exited yet
    - -1: error no children

* zombie
    a child process that has exited but parent has not called wait/waitpid yet; this consumes table slot

* reaping (release child PID)
    calling wait/waitpid to collect the child's exit status and free kernel resources

## Built-in: cd (no forking)
- set target to directory in argv[1]
- no directory: target = Home or /
- execvp will run cd as argv[0] and argv as target

## Built-in: cd (no forking)
- only prints current working directory

## execvp
* External commands: execvp runs the program named by argv[0] and passes the entire argv array as its arguments.

    - execvp(argv[0], argv);

* Built-ins: you check argv[0] to decide which built-in to handle, then use other argv entries as needed.

    - exit → uses no args.
    - cd → uses argv[1] as the path (or HOME/“/”).
    - pwd → typically no args.- external commandsL 

Progress:
    1. REPL
    2. Currently executes simple external commands (ls, ls -l)
    3. 

	We fork to run external programs without replacing the shell.

	- execvp replaces the current process; doing it in the shell would kill the shell. fork creates a child, then the child execs the program while the parent shell stays alive.
	- Isolation: the child can change stdin/stdout via dup2, set signals, and environment without affecting the parent.
	- Control: the parent can wait (foreground) or not (background), report status, and keep the prompt responsive.
	- Concurrency: multiple children let pipelines work (each stage in its own process).
	- Robustness: crashes in the child don’t crash the shell.
============================================

## To Dos:
1. Modularize
2. Make a more robust implementation of parsing. 
3. Review Memory manangement for further improvement if any.
4. Use the Command Struct.
5. Implement I/O Redirection. 
6. File System Implementation.



Notes:
main.c  /*
- Placing Command cmd; inside the loop keeps the scope and lifetime of the struct limited to each command entered, which is good practice for clarity and memory safety
- Declaring the Command struct inside the loop allocates it on the STACK for each iteration.
- Memory is automatically reclaimed at the end of each loop iteration. Only one Command struct exists at a time, and its memory is reused on each loop.

- If fflush(stdout); did not exist, the prompt (mysh> ) might not appear immediately on the terminal. Output could be delayed until the buffer is full or a newline is printed, 
    making the shell feel unresponsive or confusing for the user. This is especially noticeable in interactive programs where timely prompts are important.
*/

parser.c/*
- Static Internal State: strtok uses a static variable inside the function to remember where it left off parsing the string
- Because this variable is shared globally by the function, if you try to parse two different strings at the same time (e.g., in nested loops or multiple threads), the function gets confused and mixes up the pointers.
- sage: You pass NULL in subsequent calls to tell it "continue using the static pointer you saved last time.


- the _r stands for reentrant. Instead of using a hidden static variable, strtok_r requires you to provide a pointer (saveptr) to store the current position.
- This allows you to parse multiple strings simultaneously without them interfering with each other because each parsing operation has its own separate "bookmark" variable.
*/