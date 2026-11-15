#include "shell.h"
#include <readline/readline.h>
#include <readline/history.h>

int main() {
    char* cmdline;

    for (;;) {
        reap_jobs();                  // collect finished background jobs BEFORE prompting
        cmdline = readline(PROMPT);
        if (cmdline == NULL)
            break;

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

        if (cmdline[0] != '\0') {
            add_history_entry(cmdline);
            add_history(cmdline);
        }

        run_line(cmdline);
        free(cmdline);
    }

    printf("\nShell exited.\n");
    return 0;
}
