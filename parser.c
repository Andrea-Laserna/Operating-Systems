#include <stdio.h>
#include <string.h>
#include "parser.h"

void init_command(Command *cmd) {
    cmd->command = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = false;
    cmd->background = false;

    for (int i = 0; i < MAX_ARGS; i++)
        cmd->args[i] = NULL;
}

int parse_command(char *line, Command *cmd) {
    init_command(cmd);

    // Split input to remove whitespace
    // Return first token in line
    char *token = strtok(line, " \t\n");
    int arg_index = 0;

    while (token != NULL) {

        // Next token must be an input filename
        if (strcmp(token, "<") == 0) {
            // Continue tokenizing the same line from before
            token = strtok(NULL, " \t\n");
            if (!token) {
                fprintf(stderr, "mysh: missing input file\n");
                return -1;
            }
            cmd->input_file = token;
        // Next token must be an output filename
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t\n");
            if (!token) {
                fprintf(stderr, "mysh: missing output file\n");
                return -1;
            }
            cmd->output_file = token;
            cmd->append = false;
        // Neixt token must be an output filename
        } else if (strcmp(token, ">>") == 0) {
            token = strtok(NULL, " \t\n");
            if (!token) {
                fprintf(stderr, "mysh: missing output file\n");
                return -1;
            }
            cmd->output_file = token;
            cmd->append = true;
        // Background not added to args
        } else if (strcmp(token, "&") == 0) {
            cmd->background = true;
        // If token is not "<", ">", ">>", or "&", it is stored in args[0] and so on
        } else {
            cmd->args[arg_index++] = token;
        }

        // If no tokens remain return NULL end loop
        token = strtok(NULL, " \t\n");
    }
    // Terminate argv array
    cmd->args[arg_index] = NULL;
    // Command is first arg if any
    cmd->command = cmd->args[0];

    return 0;
}
