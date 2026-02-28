#include <stdio.h>
#include <stdlib.h>
// #include <sys/wait.h>

#include "main.h"
#include "parser.h"
#include "executor.h"
#include "jobs.h"

// REPL loop: parse input into Command and dispatch executor

// static void reap_background(void) {
//     // Non-blocking reap to prevent zombies from background jobs
//     int status;
//     pid_t pid;
//     while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
//     }
// }

int main(void) {
    char *line = NULL;
    size_t bufsize = 0;

    while (1) {
        // Reap any background children that finished since the last loop
        jobs_reap_finished();
        printf("mysh> ");
        // fflush(stdout);

        ssize_t nread = getline(&line, &bufsize, stdin);
        if (nread == -1) {
            printf("\n");
            break;
        }

        Command cmd;
        int parse_status = parse_command(line, &cmd);
        if (parse_status != 0) {
            if (parse_status < 0) {
                fprintf(stderr, "Failed to parse command\n");
            }
            continue;
        }
        
        // Handle empty input
        if (cmd.command == NULL) {
            free_command(&cmd);
            continue;
        }
        
        ExecStatus exec_status = execute_command(&cmd);
        free_command(&cmd);

        if (exec_status == EXEC_EXIT) {
            break;
        }
    }

    free(line);
    return EXIT_SUCCESS; 
}
