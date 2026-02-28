// Job table that gives bg processes an id
// Helps remember which pid belongs to which job
#include "jobs.h"

#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>

#define MAX_JOBS 128

typedef struct {
    int id;
    pid_t pid;
    int active; // 1 = currently used, 0 = free
} Job;

static Job jobs[MAX_JOBS];
static int next_job_id = 1;

static int find_slot(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        // Search for an entry that is free
        if (!jobs[i].active) return i;
    }
    // Table is full
    return -1;
}

// Locate which job entry  == PID from waitpid after child exits
static int find_by_pid(pid_t pid) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].active && jobs[i].pid == pid) return i;
    }
    return -1;
}

// Register a new bg job and return job id
int jobs_add(pid_t pid) {
    // Find free slot
    int slot = find_slot();
    if (slot < 0) return -1;

    // Mark it active
    jobs[slot].active = 1;
    // Store pid
    jobs[slot].pid = pid;
    // Assign new job id
    jobs[slot].id = next_job_id++;

    return jobs[slot].id;
}

void jobs_reap_finished(void) {
    int status = 0;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // For each pid reaped, find it in job table
        int idx = find_by_pid(pid);
        if (idx >= 0) {
            // Job finished mark inactive
            jobs[idx].active = 0;
        }
    }

    if (pid == -1 && errno != ECHILD) {
        perror("mysh: waitpid(WNOHANG)");
    }
}