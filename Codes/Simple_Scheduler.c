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

// add tslice to all processes in ready queue
void addWaitTime()
{
    for (int i = readyQueue.front; i < readyQueue.rear; i++)
    {
        readyQueue.processes[i].wait_time += tslice;
    }
}



// execute at most ncpu processes from ready queue
void execute()
{
    sort(&readyQueue);

    while (runningQueue.size != ncpu && !is_empty(&readyQueue))
    {
        process p = dequeue(&readyQueue);

        // sending SIGCONT signal
        int result = kill(p.pID, SIGCONT);

        if (result == -1)
        {
            if (errno != ESRCH)
            {
                printf("Failed to send SIGCONT");
            }

            // adding process to failed queue from ready queue
            enqueue(&failedQueue, p);
        }
        else
        {
            // adding process to running queue from ready queue
            p.cycles++;
            enqueue(&runningQueue, p);
        }
    }

    addWaitTime();
    sleepTslice();
}


int main(int argc, char const *argv[])
{
    return 0;
}
