#include "shell.h"

int main() {
    char* cmdline;
    char** arglist;

    while ((cmdline = read_cmd(PROMPT, stdin)) != NULL) {

        // Handle !n : replace the line with the n-th history command
        if (cmdline[0] == '!') {
            int n = atoi(cmdline + 1);
            const char* cmd = get_history_entry(n);
            if (cmd == NULL) {
                printf("!%s: event not found\n", cmdline + 1);
                free(cmdline);
                continue;
            }
            free(cmdline);
            cmdline = strdup(cmd);
            printf("%s\n", cmdline);
        }

        // Store non-empty commands in history
        if (cmdline[0] != '\0')
            add_history_entry(cmdline);

        if ((arglist = tokenize(cmdline)) != NULL) {
            if (handle_builtin(arglist) == 0) {
                execute(arglist);
            }
            for (int i = 0; arglist[i] != NULL; i++)
                free(arglist[i]);
            free(arglist);
        }
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
