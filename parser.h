#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

// Command representation parsed from user input
// This struct represents a command parsed from user input, including its name and arguments.
typedef struct {
    char *command;        // Command name
    char *args[256];      // Arguments (NULL-terminated)
    char *input_file;     // For < redirection (NULL if none)
    char *output_file;    // For > or >> redirection (NULL if none)
    bool append;          // true for >>, false for >
    bool background;      // true if & present
} Command;

// Parse a line into Command. Returns 0 on success, 1 on empty/whitespace, -1 on parse error.
int parse_command(const char *line, Command *cmd);

// Free any heap allocations inside Command.
void free_command(Command *cmd);

#endif
