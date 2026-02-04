#include "main.h"

int main(int argcount, char **argvect) {

    // Silening warning: not using this parameter yet
    (void)argcount;
    // Store child's exit status
    int status;

    // LOOP this

    // Child process: Launch the requested command in a separate process
    if (fork() == 0)
        /*
            Replace process with the same process id but different contents 
            argvect[0] = program name: shell 
            argvect[1] = command
            argvect[2] = command's arguments
        */ 
        execvp(argvect[1], argvect + 1);
    
    // Parent waits for the child
    wait(&status);

    return EXIT_SUCCESS; 
}