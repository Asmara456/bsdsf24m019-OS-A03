#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "FCIT> "
#define HISTORY_SIZE 20

// Function prototypes
char* read_cmd(char* prompt, FILE* fp);
char** tokenize(char* cmdline);
int execute(char** arglist, int background, const char* cmdline);
int handle_builtin(char** arglist);
void add_history_entry(const char* cmdline);
const char* get_history_entry(int n);
void print_history(void);

// Feature 6
void add_job(pid_t pid, const char* cmd);
void reap_jobs(void);
void print_jobs(void);
int run_line(char* line);

void handle_if(const char* condition);
void set_var(const char* name, const char* value);
const char* get_var(const char* name);
void print_vars(void);
int try_assignment(char** arglist);
void expand_variables(char** arglist);
#endif // SHELL_H
