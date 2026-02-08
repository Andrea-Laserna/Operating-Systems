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
	char *buffer = NULL; 				// let getline allocate buffer since we don't know how but the input is anyway

	printf("parshell> ");
	getline(&buffer, &bufsize, stdin);

	printf("You entered: %s", buffer);
	free(buffer);


	return 0; 
}
