#ifndef CIRCUITROUTER_SHELL_H
#define CIRCUITROUTER_SHELL_H

#include "lib/vector.h"
#include <sys/types.h>
#include "lib/timer.h"

#define BUFFER_SIZE 1024

typedef struct {
	pid_t pid;
	int status;
	char *pathPipe;
	TIMER_T start_time;
	TIMER_T stop_time;
} child_t;

typedef struct {
	char pipe[BUFFER_SIZE];
	char command[BUFFER_SIZE];
} msg_protocol;

void waitForChild(vector_t *children);
void printChildren(vector_t *children);

#endif /* CIRCUITROUTER_SHELL_H */
