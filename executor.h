#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

// Execution layer: run built-ins or fork/exec external commands

typedef enum {
    EXEC_OK = 0,            // Command executed successfully
    EXEC_EXIT = 1,          // Built-in 'exit' command
    EXEC_ERROR = -1         // Error during execution
} ExecStatus;

ExecStatus execute_command(const Command *cmd);

#endif
