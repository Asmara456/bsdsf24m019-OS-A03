#include "shell.h"

static void free_arglist(char** arglist) {
    for (int i = 0; arglist[i] != NULL; i++)
        free(arglist[i]);
    free(arglist);
}

// Runs ONE command (no ';' inside). Returns its exit status.
static int run_command(char* cmd) {
    while (*cmd == ' ' || *cmd == '\t') cmd++;
    int len = strlen(cmd);
    while (len > 0 && (cmd[len - 1] == ' ' || cmd[len - 1] == '\t'))
        cmd[--len] = '\0';

    int background = 0;
    if (len > 0 && cmd[len - 1] == '&') {
        background = 1;
        cmd[--len] = '\0';
        while (len > 0 && (cmd[len - 1] == ' ' || cmd[len - 1] == '\t'))
            cmd[--len] = '\0';
    }

    char** arglist = tokenize(cmd);
    if (arglist == NULL)
        return 0;

    if (try_assignment(arglist)) {     // VAR=value is a built-in action
        free_arglist(arglist);
        return 0;
    }
    expand_variables(arglist);         // replace $NAME words

    int status = 0;
    if (handle_builtin(arglist) == 0)
        status = execute(arglist, background, cmd);

    free_arglist(arglist);
    return status;
}

// Runs a whole line: commands separated by ';' run one after another.
int run_line(char* line) {
    int status = 0;
    char* saveptr;
    for (char* part = strtok_r(line, ";", &saveptr);
         part != NULL;
         part = strtok_r(NULL, ";", &saveptr))
        status = run_command(part);
    return status;
}
