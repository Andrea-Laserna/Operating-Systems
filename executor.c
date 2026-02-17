#include "executor.h"
#include "builtins.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Handles built-ins or forks external commands, wiring redirection when present

static int setup_redirection(const Command *cmd) {
    // Wire stdin/stdout to files if requested
    if (cmd->input_file != NULL) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd == -1) {
            perror("open input");
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("dup2 input");
            close(fd);
            return -1;
        }
        close(fd);
    }

    if (cmd->output_file != NULL) {
        int flags = O_WRONLY | O_CREAT;
        flags |= cmd->append ? O_APPEND : O_TRUNC;
        int fd = open(cmd->output_file, flags, 0644);
        if (fd == -1) {
            perror("open output");
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("dup2 output");
            close(fd);
            return -1;
        }
        close(fd);
    }

    return 0;
}

ExecStatus execute_command(const Command *cmd) {
    if (cmd == NULL || cmd->command == NULL) return EXEC_OK;

    if (is_builtin(cmd)) {
        return run_builtin(cmd);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return EXEC_ERROR;
    }

    if (pid == 0) {
        if (setup_redirection(cmd) != 0) {
            _exit(EXIT_FAILURE);
        }
        execvp(cmd->command, cmd->args);
        perror("execvp");
        _exit(EXIT_FAILURE);
    }

    if (cmd->background) {
        printf("[bg] pid %d\n", pid);
        return EXEC_OK;
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return EXEC_ERROR;
    }

    return EXEC_OK;
}
