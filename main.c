#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "parser.h"
#include "executor.h"

int main(void) {
    char *line = NULL;
    size_t bufsize = 0;

    while (1) {

        // Reap any finished background processes

        /*
            Check if any child process has been terminated without calling process blocking
            waitpid() = get status of child process/ dictates which child to grab from change in status
            -1        = wait for any child process
            NULL      = we are not interested on how the child died
            WNOHANG   = non-blocking or function returns immediately regardless of the child's state
            > 0       = on success, returns pid of child that change state since pid is always positive

            When a process finishes, it enters a zombie state to keep its exit status available for the parent.
            The function reaps this process by
            1. Reading the exit status of any terminated child
            2. Once read, kernel knows it no longer needs to keep the process entry and
                deletes the zombie
            3. Because of WNOHANG, parent doesnt stop to wait for active children. It only checks
                if any are already dead, reaps one if it finds it, and returns the pid

        */ 
        while (waitpid(-1, NULL, WNOHANG) > 0) {}

        printf("mysh> ");

        if (getline(&line, &bufsize, stdin) == -1) {                    // check if line from stdin failed or hit EOF
			perror("Error: check getline"); 								
            return EXIT_FAILURE;
        }

        // Parse command
        Command cmd;

        // If parsing fails, prompt again
        if (parse_command(line, &cmd) == -1)
            continue;

        // Handle empty input, prompt again
        if (cmd.command == NULL)
            continue;

        // Built in exit
        if (execute_command(&cmd) == -1)
            break;
    }

    free(line);
    return EXIT_SUCCESS;
}
