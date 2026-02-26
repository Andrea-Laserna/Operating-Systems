#define _POSIX_C_SOURCE 200809L

#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Basic tokenizer that fills Command fields (args, redirection, background).
// No quoting/escaping support yet.

void init_command(Command *cmd) {
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

// Parse line into a heap-allocated Command; caller must free_command.
Command *parse_command(const char *line, ParseStatus *status) {
    if (status) *status = PARSE_ERROR;
    if (line == NULL) return NULL;

    Command *cmd = malloc(sizeof(Command));
    if (cmd == NULL) {
        perror("malloc");
        return NULL;
    }
    init_command(cmd);

    char *buffer = strdup(line);
    if (buffer == NULL) {
        perror("strdup");
        free(cmd);
        return NULL;
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
                return NULL;
            }
            cmd->input_file = strdup(token);   // strdup to ensure we can free it later. kay pointer lang ni sng arguments, 
            if (cmd->input_file == NULL) {
                perror("strdup");
                free(buffer);
                free_command(cmd);
                return NULL;
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
                return NULL;
            }
            cmd->output_file = strdup(token);
            if (cmd->output_file == NULL) {
                perror("mysh: strdup");
                free(buffer);
                free_command(cmd);
                return NULL;
            }
            token = strtok_r(NULL, " \t\r\n", &saveptr);
            continue;
        }

        if (add_arg(cmd, token, &arg_count) != 0) {
            free(buffer);
            free_command(cmd);
            return NULL;
        }

        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }

    free(buffer);

    if (arg_count == 0) {
        free_command(cmd);
        if (status) *status = PARSE_EMPTY;
        return NULL; // empty or whitespace-only input
    }

    cmd->command = strdup(cmd->args[0]);
    if (cmd->command == NULL) {
        perror("strdup");
        free_command(cmd);
        return NULL;
    }

    if (status) *status = PARSE_OK;
    return cmd;
}

// Free Command and any heap allocations inside it.
void free_command(Command *cmd) {
    if (cmd == NULL) return;
    free(cmd->command);
    for (size_t i = 0; i < MAX_ARGS; i++) {
        free(cmd->args[i]);
    }
    free(cmd->input_file);
    free(cmd->output_file);
    free(cmd);
}


/*
help: ampangit ng code neto paulit2 pero ewan ko ba gumagana naman
*/

