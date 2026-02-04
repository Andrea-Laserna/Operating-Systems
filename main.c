#include "main.h"

int main(int argcount, char **argvect) {

    // Silening warning: not using this parameter yet
    (void)argcount;
    // Store exit status
    int status;

    // LOOP this

    // Child process: Launch the requested command in a separate process
    if (fork() == 0)
        /*
            Search PATH when given command name
            argvect[0] is shell itself
            argvect[1] is the command
            argvect[2] are the arguments

            Replace child with the requested program:
            argvect + 1 is passed to execvp so the child's argvect[0] becomes argvect[1] 

            If execvp succeeds, child process image is replaced and never reaches wait
            If execvp fails, child continues and will call wait(&status), which will fail
            because the child has no children
        */ 
        execvp(argvect[1], argvect + 1);
    
    // Parent waits for the child
    wait(&status);

    return EXIT_SUCCESS; 
}