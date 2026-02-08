/*
This is a partial shell implementation. 
It does not have a modular design, Proper header file, Command struct, etc .
This only serves as a starting point for us to test our current 
understanding of the concepts tackled in class and learned with youtube tutorials. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){

	size_t bufsize = 0;
	char *command = NULL, *cmd_copy = NULL, *token = NULL; 							// let getline allocate command since we don't know how long/big the input is anyway
	char *delimiter = " \n";
	int argc = 0, i = 0; 										
	char **argv = NULL;

	//read command
	printf("parshell> ");
	if (getline(&command, &bufsize, stdin) == -1) {
		perror("getline"); 												// to know where the error is coming from
		free(command);													// Free the allocated memory to prevent memory leak
		return EXIT_FAILURE; 
	}

	//make a copy of command to cound argc
	cmd_copy = strdup(command);											// since the strtok modifies the original string, we need to make a copy of it to use for tokenization. strdup allocates memory for the copy, so we need to free it later.
	token = strtok(cmd_copy, delimiter);								// returns a pointer to a null-terminated string containing the next token.	
	while (token != NULL) {												
        argc++;
        token = strtok(NULL, delimiter);
    }
	printf("argc: %d\n", argc);
	free(cmd_copy);

	//allocate memory for argv using argc and fill with tokens from command
	argv = malloc(sizeof(char *) * (argc + 1));   						// + 1 for the NULL terminator   				
    token = strtok(command, delimiter);
    while (token != NULL) {
        argv[i] = token;        
        token = strtok(NULL, delimiter);           
        i++;
    }
    argv[i] = NULL;                            

	i = 0;
    while (argv[i] != NULL) {
		printf("argv[%d]: %s\n", i, argv[i]);
		i++;
    }
	
	free(command);														// reminder to not free command until after done using the tokens
	free(argv);															
	return EXIT_SUCCESS; 
}


/*
Design Decisions: 
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
*/