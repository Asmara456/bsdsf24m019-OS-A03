#include "shell.h"
#include <readline/readline.h>

#define MAX_BLOCK 50

static char* trim(char* s) {
    while (*s == ' ' || *s == '\t') s++;
    int len = strlen(s);
    while (len > 0 && (s[len - 1] == ' ' || s[len - 1] == '\t'))
        s[--len] = '\0';
    return s;
}

// 'condition' is the text after the word "if"
void handle_if(const char* condition) {
    char* cond = strdup(condition);
    char* then_lines[MAX_BLOCK];
    char* else_lines[MAX_BLOCK];
    int nthen = 0, nelse = 0;
    int section = 0;      // 0 = waiting for 'then', 1 = then-block, 2 = else-block
    int finished = 0, ok = 1;

    if (trim(cond)[0] == '\0') {
        fprintf(stderr, "syntax error: 'if' needs a command\n");
        free(cond);
        return;
    }

    while (!finished) {
        char* line = readline("> ");
        if (line == NULL) {
            fprintf(stderr, "syntax error: unexpected end of input (missing 'fi')\n");
            ok = 0;
            break;
        }
        char* t = trim(line);

        if (strcmp(t, "fi") == 0) {
            if (section == 0) {
                fprintf(stderr, "syntax error: 'fi' before 'then'\n");
                ok = 0;
            }
            finished = 1;
        } else if (strcmp(t, "then") == 0 && section == 0) {
            section = 1;
        } else if (strcmp(t, "else") == 0 && section == 1) {
            section = 2;
        } else if (section == 0) {
            fprintf(stderr, "syntax error: expected 'then'\n");
            ok = 0;
        } else if (section == 1 && nthen < MAX_BLOCK) {
            then_lines[nthen++] = strdup(t);
        } else if (section == 2 && nelse < MAX_BLOCK) {
            else_lines[nelse++] = strdup(t);
        }
        free(line);
    }

    if (ok) {
        int status = run_line(cond);          // the 'if' command
        if (status == 0) {
            for (int i = 0; i < nthen; i++)
                run_line(then_lines[i]);
        } else {
            for (int i = 0; i < nelse; i++)
                run_line(else_lines[i]);
        }
    }

    free(cond);
    for (int i = 0; i < nthen; i++) free(then_lines[i]);
    for (int i = 0; i < nelse; i++) free(else_lines[i]);
}
