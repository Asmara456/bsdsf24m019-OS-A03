#include "shell.h"

char* read_cmd(char* prompt, FILE* fp) {
    printf("%s", prompt);
    char* cmdline = (char*) malloc(sizeof(char) * MAX_LEN);
    int c, pos = 0;

    while ((c = getc(fp)) != EOF) {
        if (c == '\n') break;
        cmdline[pos++] = c;
    }

    if (c == EOF && pos == 0) {
        free(cmdline);
        return NULL; // Handle Ctrl+D
    }
    
    cmdline[pos] = '\0';
    return cmdline;
}


int handle_builtin(char** arglist) {
    if (arglist[0] == NULL)
        return 0;

    if (strcmp(arglist[0], "exit") == 0) {
        printf("Shell exited.\n");
        exit(0);
    }

    if (strcmp(arglist[0], "cd") == 0) {
        if (arglist[1] == NULL || arglist[1][0] == '\0') {
            char* home = getenv("HOME");
            if (home == NULL || chdir(home) != 0)
                perror("cd");
        } else if (chdir(arglist[1]) != 0) {
            perror("cd");
        }
        return 1;
    }

    if (strcmp(arglist[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("  exit        - terminate the shell\n");
        printf("  cd <dir>    - change the current directory\n");
        printf("  help        - show this help message\n");
        printf("  jobs        - list background jobs\n");
        printf("  set         - show all shell variables\n");
        return 1;
    }

    if (strcmp(arglist[0], "jobs") == 0) {
        print_jobs();
        return 1;
    }

    if (strcmp(arglist[0], "history") == 0) {
        print_history();
        return 1;
    }

    if (strcmp(arglist[0], "set") == 0) {
        print_vars();
        return 1;
    }

    return 0;
}

// ---------- Command history ----------
static char* history[HISTORY_SIZE];
static int hist_total = 0;   // total commands ever added

void add_history_entry(const char* cmdline) {
    int idx = hist_total % HISTORY_SIZE;
    free(history[idx]);              // free the old entry (NULL is safe)
    history[idx] = strdup(cmdline);
    hist_total++;
}

// n is 1-based and refers to the numbering shown by 'history'
const char* get_history_entry(int n) {
    int first = (hist_total > HISTORY_SIZE) ? hist_total - HISTORY_SIZE + 1 : 1;
    if (n < first || n > hist_total)
        return NULL;
    return history[(n - 1) % HISTORY_SIZE];
}

void print_history(void) {
    int first = (hist_total > HISTORY_SIZE) ? hist_total - HISTORY_SIZE + 1 : 1;
    for (int i = first; i <= hist_total; i++)
        printf("%4d  %s\n", i, history[(i - 1) % HISTORY_SIZE]);
}
