#include "builtins.h"
#include "main.h"

// Minimal built-in implementations (cd, pwd, exit)

int is_builtin(const Command *cmd) {
    if (cmd == NULL || cmd->command == NULL) return 0;
    return strcmp(cmd->command, "cd") == 0 || strcmp(cmd->command, "pwd") == 0 || strcmp(cmd->command, "exit") == 0;
}

static ExecStatus builtin_cd(const Command *cmd) {
    const char *target = NULL;
    if (cmd->args[1] != NULL) {
        target = cmd->args[1];
    } else {
        target = getenv("HOME");
        if (target == NULL) target = "/";
    }
    if (chdir(target) == -1) {
        perror("cd");
        return EXEC_ERROR;
    }
    return EXEC_OK;
}

static ExecStatus builtin_pwd(void) {
    char *cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        perror("pwd");
        return EXEC_ERROR;
    }
    printf("%s\n", cwd);
    free(cwd);
    return EXEC_OK;
}

ExecStatus run_builtin(const Command *cmd) {
    if (cmd == NULL || cmd->command == NULL) return EXEC_OK;

    if (strcmp(cmd->command, "exit") == 0) {
        return EXEC_EXIT;
    }

    if (strcmp(cmd->command, "cd") == 0) {
        return builtin_cd(cmd);
    }

    if (strcmp(cmd->command, "pwd") == 0) {
        return builtin_pwd();
    }

    return EXEC_OK;
}
