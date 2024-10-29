#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <poll.h>
#include <string.h>

#include "message.h"
#include "Queue.h"

int ncpu, tslice;

char *pipeName = "/tmp/Simple_Scheduler_pipe";

Queue readyQueue;
Queue runningQueue;
Queue completedQueue;
Queue failedQueue;


int main(int argc, char const *argv[])
{
    return 0;
}
