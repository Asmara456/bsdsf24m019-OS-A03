#include "shell.h"

#define MAX_JOBS 64

struct job {
    pid_t pid;
    char cmd[MAX_LEN];
};

static struct job jobs[MAX_JOBS];
static int njobs = 0;

void add_job(pid_t pid, const char* cmd) {
    if (njobs >= MAX_JOBS) {
        fprintf(stderr, "too many background jobs\n");
        return;
    }
    jobs[njobs].pid = pid;
    strncpy(jobs[njobs].cmd, cmd, MAX_LEN - 1);
    jobs[njobs].cmd[MAX_LEN - 1] = '\0';
    njobs++;
}

static void remove_job(pid_t pid) {
    for (int i = 0; i < njobs; i++) {
        if (jobs[i].pid == pid) {
            for (int j = i; j < njobs - 1; j++)
                jobs[j] = jobs[j + 1];
            njobs--;
            return;
        }
    }
}

// Collect finished children without blocking (this removes zombies)
void reap_jobs(void) {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        remove_job(pid);
}

void print_jobs(void) {
    reap_jobs();
    if (njobs == 0) {
        printf("No background jobs.\n");
        return;
    }
    for (int i = 0; i < njobs; i++)
        printf("[%d] PID %d  Running  %s\n", i + 1, jobs[i].pid, jobs[i].cmd);
}
