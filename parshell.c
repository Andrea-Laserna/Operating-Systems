/*
This is a partial shell implementation. 
It does not have a modular design, Proper header file, Command struct, etc .
This only serves as a starting point for us to test our current 
understanding of the concepts tackled in class and learned with youtube tutorials. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
	size_t bufsize = 0; 
	char *command = NULL, *cmd_copy = NULL, *token = NULL; 							
	char *delimiter = " \n";
	int argc, i; 										
	char **argv = NULL;
	pid_t pid;															

	//loop to read and execute commands until user exits
	while (1) {
		argc = 0;
		i = 0;

		//read command
		printf("parshell> ");
		if (getline(&command, &bufsize, stdin) == -1) {						// check if line from stdin failed or hit EOF
			perror("Error: check getline"); 								// to know where the error is coming from
			free(command);													// Free the allocated memory to prevent memory leak
			return EXIT_FAILURE; 
		}
	
		//make a copy of command to count argc
		cmd_copy = strdup(command);											// since the strtok modifies the original string, we need to make a copy of it to use for tokenization. strdup allocates memory for the copy, so we need to free it later.
		token = strtok(cmd_copy, delimiter);								// returns a pointer to a null-terminated string containing the next token.	
		while (token != NULL) {												
			argc++;
			token = strtok(NULL, delimiter);								// tokenize remaining commands,then null
		}
		//printf("argc: %d\n", argc);
		free(cmd_copy);
	
		//allocate memory for argv using argc and fill with tokens from command
		argv = malloc(sizeof(char *) * (argc + 1));   						// + 1 for the NULL terminator for the exec				
		token = strtok(command, delimiter);
		while (token != NULL) {
			argv[i] = token;        
			token = strtok(NULL, delimiter);           
			i++;
		}
		argv[i] = NULL;                            

		// handle empty input
		if (argc == 0 || argv[0] == NULL) {
			free(argv);
			continue;
		}

		// built-in: exit (no forking)
		if (strcmp(argv[0], "exit") == 0) {
			// reap any finished children to avoid zombies
			while (waitpid(-1, NULL, WNOHANG) > 0) {}		// target any child, ignore exit status, wnohang flag: waitpid returns immediately
			free(argv);
			break;
		}

		// built-in: cd (no forking)
		if (strcmp(argv[0], "cd") == 0) {
			const char *target = NULL;						// path: no string yet
			// command + directory
			if (argc >= 2 && argv[1] != NULL) {				
				target = argv[1];							// point target to directory
			// no directory -> return Home or /
			} else {
				target = getenv("HOME");
				if (target == NULL) target = "/";
			}
			if (chdir(target) == -1) {
				perror("Error: cd");
			}
			free(argv);
			continue;
		}

		// built-in: pwd (no forking)
		if (strcmp(argv[0], "pwd") == 0) {
			char *cwd = getcwd(NULL, 0);
			if (cwd == NULL) {
				perror("Error: pwd");
			} else {
				printf("%s\n", cwd);
				free(cwd);
			}
			free(argv);
			continue;
		}

		//fork and exec for external commands
		pid = fork();
		if (pid == -1) {
			perror("Error: Fork failed");
			free(argv);
			return EXIT_FAILURE;
		}

		if (pid == 0) {
			int val = execvp(argv[0], argv); 
			if (val == -1) perror("execvp failed");
			exit(EXIT_FAILURE);
		} else {
			wait(NULL);
			free(argv);
			//printf("Done with execvp \n");
		}
			
	}
		
		free(command);														// reminder to not free command until after done using the tokens

	return EXIT_SUCCESS; 
}

