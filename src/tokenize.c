#include "shell.h"

// Splits a command line into words. Text inside "..." or '...' stays one
// word (quotes removed). Each word is malloc'd; the list ends with NULL.
char** tokenize(char* cmdline) {
    if (cmdline == NULL || cmdline[0] == '\0')
        return NULL;

    char** arglist = malloc(sizeof(char*) * (MAXARGS + 1));
    int argnum = 0;
    char* cp = cmdline;

    while (*cp != '\0' && argnum < MAXARGS) {
        while (*cp == ' ' || *cp == '\t') cp++;
        if (*cp == '\0') break;

        char* buf = malloc(strlen(cp) + 1);
        int len = 0;
        char quote = 0;

        while (*cp != '\0') {
            if (quote) {
                if (*cp == quote) quote = 0;
                else buf[len++] = *cp;
            } else if (*cp == '"' || *cp == '\'') {
                quote = *cp;
            } else if (*cp == ' ' || *cp == '\t') {
                break;
            } else {
                buf[len++] = *cp;
            }
            cp++;
        }
        buf[len] = '\0';
        arglist[argnum++] = buf;
    }

    arglist[argnum] = NULL;
    if (argnum == 0) {
        free(arglist);
        return NULL;
    }
    return arglist;
}
