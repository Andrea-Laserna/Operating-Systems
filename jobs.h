#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h> // pid_t

// Register a new background job. Returns a positive job id, or -1 on failure.
int jobs_add(pid_t pid);

// Reap any finished children without blocking (prevents zombies).
// If the PID belongs to a tracked job, the job is removed from the job table.
void jobs_reap_finished(void);

#endif