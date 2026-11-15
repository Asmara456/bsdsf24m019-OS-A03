#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    char* cmdline;
    char** arglist;

    // readline() prints the prompt, handles line editing, arrow keys and Tab.
    // It returns a malloc'd string without the newline, or NULL on Ctrl+D.
    while ((cmdline = readline(PROMPT)) != NULL) {

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
            printf("%s\n", cmdline);   // show what is being re-executed
        }

        // Store non-empty commands in both histories
        if (cmdline[0] != '\0') {
            add_history_entry(cmdline);   // our own list (history built-in, !n)
            add_history(cmdline);         // Readline's list (Up/Down arrows)
        }

        if ((arglist = tokenize(cmdline)) != NULL) {
            if (handle_builtin(arglist) == 0) {
                execute(arglist);
            }

            // Free the memory allocated by tokenize()
            for (int i = 0; arglist[i] != NULL; i++) {
                free(arglist[i]);
            }
            free(arglist);
        }
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
