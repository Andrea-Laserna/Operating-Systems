#define _POSIX_C_SOURCE 200809L

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Basic tokenizer that fills Command fields (args, redirection, background).
// No quoting/escaping support yet.

static void init_command(Command *cmd) {
    // Reset all fields and clear argument slots
    cmd->command = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append = false;
    cmd->background = false;

    for (int i = 0; i < MAX_ARGS; i++)
        cmd->args[i] = NULL;
}

static int add_arg(Command *cmd, const char *token, size_t *arg_count) {
    // Append a duplicated token into args, enforcing a fixed max
    if (*arg_count >= MAX_ARGS - 1) {
        fprintf(stderr, "Too many arguments (max 255)\n");
        return -1;
    }
    cmd->args[*arg_count] = strdup(token);
    if (cmd->args[*arg_count] == NULL) {
        perror("strdup");
        return -1;
    }
    (*arg_count)++;
    cmd->args[*arg_count] = NULL;
    return 0;
}

// Returns:
//  0  success (cmd populated)
//  1  empty/whitespace-only input
// -1  parse error

int parse_command(const char *line, Command *cmd) {
    if (line == NULL || cmd == NULL) return -1;

    init_command(cmd);

    char *buffer = strdup(line);
    if (buffer == NULL) {
        perror("strdup");
        return -1;
    }

    size_t arg_count = 0;
    char *saveptr = NULL;
    char *token = strtok_r(buffer, " \t\r\n", &saveptr);

    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            cmd->background = true;
            token = strtok_r(NULL, " \t\r\n", &saveptr);
            continue;
        }

        if (strcmp(token, "<") == 0) {
            token = strtok_r(NULL, " \t\r\n", &saveptr);
            if (token == NULL) {
                fprintf(stderr, "Missing input file after '<'\n");
                free(buffer);
                free_command(cmd);
                return -1;
            }
            cmd->input_file = strdup(token);
            if (cmd->input_file == NULL) {
                perror("strdup");
                free(buffer);
                free_command(cmd);
                return -1;
            }
            token = strtok_r(NULL, " \t\r\n", &saveptr);
            continue;
        }

        if (strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
            cmd->append = (strcmp(token, ">>") == 0);
          
            token = strtok_r(NULL, " \t\r\n", &saveptr);
          
            if (token == NULL) {
                fprintf(stderr, "Missing output file after '>'\n");
                free(buffer);
                free_command(cmd);
                return -1;
            }
            cmd->output_file = strdup(token);
            if (cmd->output_file == NULL) {
                perror("mysh: strdup");
                free(buffer);
                free_command(cmd);
                return -1;
            }
            token = strtok_r(NULL, " \t\r\n", &saveptr);
            continue;
        }

        if (add_arg(cmd, token, &arg_count) != 0) {
            free(buffer);
            free_command(cmd);
            return -1;
        }

        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }

    free(buffer);

    if (arg_count == 0) {
        free_command(cmd);
        return 1; // empty or whitespace-only input
    }

    cmd->command = strdup(cmd->args[0]);
    if (cmd->command == NULL) {
        perror("strdup");
        free_command(cmd);
        return -1;
    }

    return 0;
}

// Free any heap allocations inside Command.
void free_command(Command *cmd) {
    if (cmd == NULL) return;
    free(cmd->command);
    for (size_t i = 0; i < MAX_ARGS; i++) {
        free(cmd->args[i]);
        cmd->args[i] = NULL;
    }
    free(cmd->input_file);
    free(cmd->output_file);
    cmd->input_file = NULL;
    cmd->output_file = NULL;
  
    cmd->append = false;
    cmd->background = false;
}


/*
help: ampangit ng code neto paulit2 pero ewan ko ba gumagana naman
*/

