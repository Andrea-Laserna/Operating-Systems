/*
This is a partial shell implementation. 
It does not have a modular design, Proper header file, Command struct, etc .
This only serves as a starting point for us to test our current 
understanding of the concepts tackled in class and learned with youtube tutorials. 
*/

#include <stdio.h>
#include <stdlib.h>

int main(void){

	size_t bufsize = 0;
	char *buffer = NULL; 							// let getline allocate buffer since we don't know how but the input is anyway



	printf("parshell> ");
	if (getline(&buffer, &bufsize, stdin) == -1) {
		perror("getline"); 							// to know where the error is coming from
		free(buffer);
		return EXIT_FAILURE; 

	}

	printf("You entered: %s", buffer);
	free(buffer);


	return 0; 
}


/*
Design Decisions: 
1. why use getline?
	- It is a convenient way to read an entire line of input, regardless of its length.
	- It automatically allocates a buffer and resizes it as needed, which simplifies memory management.
	- It handles the newline character at the end of the input, which can be useful for processing commands.
*/