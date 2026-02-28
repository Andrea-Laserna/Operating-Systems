#include "main.h"
#include "parser.h"
#include "executor.h"
#include "jobs.h"

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

        ParseStatus parse_status = PARSE_ERROR;
        Command *cmd = parse_command(line, &parse_status);
        if (parse_status == PARSE_EMPTY) {
            continue;
        }
        if (cmd == NULL) {
            fprintf(stderr, "Failed to parse command\n");
            continue;
        }
        
        ExecStatus exec_status = execute_command(cmd);
        free_command(cmd);

        if (exec_status == EXEC_EXIT) {
            break;
        }
    }

    free(line);
    return EXIT_SUCCESS; 
}
