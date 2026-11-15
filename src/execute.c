#include "shell.h"
#include <fcntl.h>

// Runs in the CHILD: handles < and >, then removes those tokens.
static void setup_redirection(char** cmd) {
    for (int i = 0; cmd[i] != NULL; i++) {
        if (strcmp(cmd[i], "<") == 0 || strcmp(cmd[i], ">") == 0) {
            int is_input = (cmd[i][0] == '<');

            if (cmd[i + 1] == NULL) {
                fprintf(stderr, "syntax error: expected filename after '%s'\n", cmd[i]);
                exit(1);
            }

            int fd;
            if (is_input)
                fd = open(cmd[i + 1], O_RDONLY);
            else
                fd = open(cmd[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (fd < 0) {
                perror(cmd[i + 1]);
                exit(1);
            }

            dup2(fd, is_input ? STDIN_FILENO : STDOUT_FILENO);
            close(fd);

            // Remove the operator and filename by shifting the rest left
            int j = i;
            while (cmd[j + 2] != NULL) {
                cmd[j] = cmd[j + 2];
                j++;
            }
            cmd[j] = NULL;
            cmd[j + 1] = NULL;
            i--;
        }
    }
}

static void run_in_child(char** cmd) {
    setup_redirection(cmd);
    if (cmd[0] == NULL)
        exit(1);
    execvp(cmd[0], cmd);
    perror("Command not found");
    exit(1);
}

int execute(char** arglist) {
    int pipe_idx = -1;
    for (int i = 0; arglist[i] != NULL; i++) {
        if (strcmp(arglist[i], "|") == 0) {
            pipe_idx = i;
            break;
        }
    }

    // ---- No pipe: single command ----
    if (pipe_idx == -1) {
        int status;
        pid_t cpid = fork();
        if (cpid == -1) {
            perror("fork failed");
            return -1;
        }
        if (cpid == 0)
            run_in_child(arglist);
        waitpid(cpid, &status, 0);
        return 0;
    }

    // ---- Pipe: left | right ----
    if (pipe_idx == 0 || arglist[pipe_idx + 1] == NULL) {
        fprintf(stderr, "syntax error near '|'\n");
        return -1;
    }

    char* left[MAXARGS + 1];
    for (int i = 0; i < pipe_idx; i++)
        left[i] = arglist[i];
    left[pipe_idx] = NULL;
    char** right = &arglist[pipe_idx + 1];

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe failed");
        return -1;
    }

    pid_t p1 = fork();
    if (p1 == 0) {                 // left child: writer
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        run_in_child(left);
    }

    pid_t p2 = fork();
    if (p2 == 0) {                 // right child: reader
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        run_in_child(right);
    }

    close(fd[0]);                  // parent must close both ends
    close(fd[1]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    return 0;
}
