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
	char *command = NULL; 							// let getline allocate buffer since we don't know how but the input is anyway
	char *delimiter = " \n";
	char *token; 


	printf("parshell> ");
	if (getline(&command, &bufsize, stdin) == -1) {
		perror("getline"); 							// to know where the error is coming from
		free(command);								// Free the allocated memory to prevent memory leak
		return EXIT_FAILURE; 
	}

	token = strtok(command, delimiter);				// returns a pointer to a null-terminated string containing the next token.
	while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, delimiter);
    }


	printf("You entered: %s", command);
	free(command);


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

*/