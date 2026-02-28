#include "executor.h"
#include "builtins.h"
#include "jobs.h"

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
            perror("mysh: input file");
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("mysh: dup2 input");
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

    // Builtins are handled without forking
    if (is_builtin(cmd)) {
        return run_builtin(cmd);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("mysh: fork");
        return EXEC_ERROR;
    }

    if (pid == 0) {
        if (setup_redirection(cmd) != 0) {
            _exit(EXIT_FAILURE);
        }
      
        execvp(cmd->command, cmd->args);
      
        // execvp only returns on failure
        if (errno == ENOENT) {
            fprintf(stderr, "mysh: command not found: %s\n", cmd->command);
        } else {
            perror("mysh: exec");
        }
        _exit(127);
    }

    if (cmd->background) {
        int jid = jobs_add(pid);
        if (jid < 0) {
            printf("[PID %d] running in background\n", pid);
        } else {
            printf("[%d] %d\n", jid, pid);
        }
        return EXEC_OK;
    }

    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
        perror("mysh: waitpid");
        return EXEC_ERROR;
    }

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        if (code != 0) {
            fprintf(stderr, "mysh: process %d exited with status %d\n", pid, code);
        }
    } else if (WIFSIGNALED(status)) {
        int sig = WTERMSIG(status);
        fprintf(stderr, "mysh: process %d terminated by signal %d\n", pid, sig);
    }

    return EXEC_OK;
}
