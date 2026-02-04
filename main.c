#include "main.h"

int main(int argcount, char **argvect) {

    (void)argcount;
    int status;

    // LOOP this

    // Child process
    if (fork() == 0)
        execvp(argvect[1], argvect + 1);
    
    // Parent waits for the child and stalk inside status the return value of the child
    wait(&status);

    return EXIT_SUCCESS; 
}