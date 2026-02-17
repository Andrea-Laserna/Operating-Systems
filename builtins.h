#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"
#include "executor.h"

// Built-in command detection and dispatch (cd, pwd, exit)

int is_builtin(const Command *cmd);
ExecStatus run_builtin(const Command *cmd);

#endif
