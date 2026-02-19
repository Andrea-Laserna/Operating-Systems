#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#include "executor.h"

/*
    Executes a parsed command
    0  -> continue shell
    -1  -> exit shell
 */
int execute_command(Command *cmd) {

    // Built-in exit
    if (strcmp(cmd->command, "exit") == 0) {

        // Reap any remaining children to avoid zombies
        while (waitpid(-1, NULL, WNOHANG) > 0) {}
        // Signal main to terminate shell
        return -1;  
    }

    // Built-in cd
    if (strcmp(cmd->command, "cd") == 0) {

        const char *target = cmd->args[1];

        if (target == NULL) {
            target = getenv("HOME");
            if (target == NULL)
                target = "/";
        }

        if (chdir(target) == -1) {
            perror("mysh: cd");
        }

        return 0;
    }

    // Built-in pwd
    if (strcmp(cmd->command, "pwd") == 0) {

        char *cwd = getcwd(NULL, 0);

        if (cwd == NULL) {
            perror("mysh: pwd");
        } else {
            printf("%s\n", cwd);
            free(cwd);
        }

        return 0;
    }

    // External command: run external programs without replacing the shell
    pid_t pid = fork();

    if (pid < 0) {
        perror("mysh: fork");
        return 0;
    }

    // Child Process
    if (pid == 0) {

        // Input redirection
        if (cmd->input_file) {
            /*
            File descriptor fd = open() returns int in which external programs will read from
                1. Command reads from the terminal (fd 0)
                2. dup2 makes fd 0 refer to input file
                3. Performing execvp will make the command read from the input file
            */ 
            
            // Establish the connection between file and file descriptor
            int fd = open(cmd->input_file, O_RDONLY);
            if (fd < 0) {
                perror("mysh: input file");
                exit(EXIT_FAILURE);
            }
            /*
                Replace fd 0 so child's stdin becomes the file
                    0 = stdin
                    1 = stdout
                    2 = stderr
                Programs read from stdin by default. If you just open a file
                and get fd 3, the program will still read from fd 0 (terminal).

                dup2(fd, 0) reassigns stdin (fd 0) to point at the opened file so
                the program’s normal stdin reads from that file.
            */ 
            if (dup2(fd, STDIN_FILENO) < 0) {
                perror("mysh: dup2 input");
                close(fd);
                exit(EXIT_FAILURE);
            }
            // Close original fd to avoid leaks
            close(fd);
        }

        // Output Redirection
        if (cmd->output_file) {
            /*
            1. File descriptor outputs to stdout (fd 1)
            2. dup2 makes fd 1 refer to output file
            3. Performing execvp will make the command read from the input file
            */ 

            // Open for writing, Create for missing
            int flags = O_WRONLY | O_CREAT;
            // Append to existing file or overwrite/empty the file
            flags |= (cmd->append) ? O_APPEND : O_TRUNC;

            // If O_CREAT is in flags, owner can read and write, group can read, others can read 
            int fd = open(cmd->output_file, flags, 0644);
            if (fd < 0) {
                perror("mysh: output file");
                exit(EXIT_FAILURE);
            }

            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("mysh: dup2 output");
                close(fd);
                exit(EXIT_FAILURE);
            }

            close(fd);
        }

        // Execute program
        execvp(cmd->command, cmd->args);

        // execvp returns = fail/File did not exist
        if (errno == ENOENT) {
            fprintf(stderr, "mysh: command not found: %s\n", cmd->command);
        } else {
            perror("mysh: exec");
        }

        // Terminate program with 127 exit status - exec not found
        exit(127);
    }

    // Parent Process
    /*
        Background processes - commands the shell starts but doesnt wait for
        The shell immediately returns to prompt so we can run other commands while they run

        Foreground - shells waits, process has terminal for stdin
                    - blocks until child finishes, then reports exit/signal
        Background - shell does not wait, process shouldnt read from stdin. It writes to stdout or stderr
                    - skips waiting, immediately returns to the prompt
    */ 
    else {
        // Shell runs the child in the foreground
        if (!cmd->background) {
            // Wait status for the child
            int status;
            // Block until child finishes
            if (waitpid(pid, &status, 0) == -1) {
                perror("mysh: waitpid");
            } else {
                // Return true if child terminated normally (via exit or return from main)
                if (WIFEXITED(status)) {
                    // Give exit code - If non-zero print message
                    int code = WEXITSTATUS(status);

                    if (code != 0) {
                        fprintf(stderr, "mysh: process %d exited with status %d\n", pid, code);
                    }
                }
                // Child was killed by uncaught signal
                else if (WIFSIGNALED(status)) {
                    // Signal number 
                    int sig = WTERMSIG(status);
                    fprintf(stderr, "mysh: process %d terminated by signal %d\n", pid, sig);
                }
            }
        // Shell does not wait, print pid and return to prompt 
        } else {
            printf("[PID %d] running in background\n", pid);
        }
    }

    return 0;
}
