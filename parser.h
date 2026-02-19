#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

#define MAX_ARGS 256

typedef struct {
	char *command;            // argv[0]
	char *args[MAX_ARGS];     // NULL-terminated argument vector for execvp
	char *input_file;         // for '<' (parsed, not yet applied)
	char *output_file;        // for '>' or '>>' (parsed, not yet applied)
	int append;               // 1 if '>>'(append), 0 if '>'(truncate)
	int background;           // 1 if '&' is the last token
} Command;

void init_command(Command *cmd);
int parse_command(char *line, Command *cmd);

#endif
