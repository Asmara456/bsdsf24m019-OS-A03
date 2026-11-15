#include "shell.h"
#include <ctype.h>

struct var {
    char* name;
    char* value;
    struct var* next;
};

static struct var* head = NULL;

void set_var(const char* name, const char* value) {
    for (struct var* v = head; v != NULL; v = v->next) {
        if (strcmp(v->name, name) == 0) {      // exists: replace the value
            free(v->value);
            v->value = strdup(value);
            return;
        }
    }
    struct var* n = malloc(sizeof(struct var));  // new: add at the front
    n->name = strdup(name);
    n->value = strdup(value);
    n->next = head;
    head = n;
}

const char* get_var(const char* name) {
    for (struct var* v = head; v != NULL; v = v->next)
        if (strcmp(v->name, name) == 0)
            return v->value;
    return NULL;
}

void print_vars(void) {
    for (struct var* v = head; v != NULL; v = v->next)
        printf("%s=%s\n", v->name, v->value);
}

// Detects NAME=value (one word, no spaces around '='). Returns 1 if handled.
int try_assignment(char** arglist) {
    if (arglist[1] != NULL)
        return 0;
    char* eq = strchr(arglist[0], '=');
    if (eq == NULL || eq == arglist[0])
        return 0;
    if (isdigit((unsigned char)arglist[0][0]))
        return 0;
    for (char* p = arglist[0]; p < eq; p++)
        if (!(isalnum((unsigned char)*p) || *p == '_'))
            return 0;

    *eq = '\0';                       // split into name and value
    set_var(arglist[0], eq + 1);
    return 1;
}

// Replaces every word starting with '$' by the variable's value
void expand_variables(char** arglist) {
    for (int i = 0; arglist[i] != NULL; i++) {
        if (arglist[i][0] == '$' && arglist[i][1] != '\0') {
            const char* val = get_var(arglist[i] + 1);
            if (val == NULL)
                val = getenv(arglist[i] + 1);          // fall back to $HOME, $USER...
            char* expanded = strdup(val ? val : "");   // new memory
            free(arglist[i]);                          // free the old word
            arglist[i] = expanded;
        }
    }
}
