#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#define MAX_ARGS 255

typedef struct {
	char *command;            // argv[0]
	char *args[MAX_ARGS];     // NULL-terminated argument vector for execvp
	char *input_file;         // for '<' (parsed, not yet applied)
	char *output_file;        // for '>' or '>>' (parsed, not yet applied)
	int append;               // 1 if '>>'(append), 0 if '>'(truncate)
	int background;           // 1 if '&' is the last token
} Command;

typedef enum {
	PARSE_OK = 0,
	PARSE_EMPTY = 1,
	PARSE_ERROR = -1
} ParseStatus;

void init_command(Command *cmd);

// Parse a line into a heap-allocated Command. Caller must free_command() the result.
// Returns Command* on success (status PARSE_OK), NULL on empty input (status PARSE_EMPTY)
// or parse failure (status PARSE_ERROR).
Command *parse_command(const char *line, ParseStatus *status);

// Free Command and any heap allocations inside it (expects a parse_command() result).
void free_command(Command *cmd);

#endif
