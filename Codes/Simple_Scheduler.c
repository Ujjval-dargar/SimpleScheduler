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

// stop all running processes
void stopRunningProcesses()
{
    for (int i = runningQueue.front; i < runningQueue.rear; i++)
    {
        process p = dequeue(&runningQueue);

        int pID = p.pID;

        // sending SIGSTOP signal to running process
        if (kill(pID, SIGSTOP) == -1)
        {
            if (errno == ESRCH)
            {
                if (gettimeofday(&p.end_time, NULL) != 0)
                {
                    printf("Failed to get time.\n");
                    exit(0);
                }

                // adding to completed queue
                enqueue(&completedQueue, p);
            }
            else
            {
                printf("Failed to stop process %d.\n", pID);
                exit(0);
            }
        }
        else
        {
            // adding to ready queue
            enqueue(&readyQueue, p);
        }
    }

    // Reset runningQueue
    clear(&runningQueue);
}

// receiving message from shell
void receiveMessage()
{
    int c;
    do
    {
        c = -1;
        const int fd = open(pipeName, O_RDONLY | O_NONBLOCK);

        if (fd == -1)
        {
            printf("Failed to open pipe.\n");
            exit(0);
        }

        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLIN;

        // checking if there is data to read or not
        int result = poll(&pfd, 1, 1);

        if (result == -1)
        {
            printf("Failed to poll.\n");
            close(fd);
            exit(0);
        }
        else if (pfd.revents & POLLIN)
        {
            struct message msg;

            ssize_t bytes_read = read(fd, &msg, sizeof(struct message));

            if (bytes_read > 0)
            {
                process p;

                p.pID = msg.pID;
                p.priority = msg.priority;
                p.cycles = 0;
                p.wait_time = 0;
                strcpy(p.command, msg.command);

                c = 0;

                if (gettimeofday(&p.arrival_time, NULL) != 0)
                {
                    printf("Failed to get time.\n");
                    exit(0);
                }

                // adding to ready queue
                enqueue(&readyQueue, p);
            }
        }

        close(fd);

    } while (c != -1);
}

// sleep scheduler for tslice milliseconds
void sleepTslice()
{
    struct timespec slice;

    slice.tv_sec = tslice / 1000;
    slice.tv_nsec = (tslice % 1000) * 1000000;

    if (nanosleep(&slice, NULL) == -1)
    {
        printf("Failed to sleep.\n");
        exit(0);
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

// handle signals
void handle_signals()
{
    // handling SIGINT signal to be ignored
    struct sigaction sig;
    sig.sa_handler = SIG_IGN;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;

    if (sigaction(SIGINT, &sig, NULL) == -1)
    {
        printf("Failed to add SIGINT handler.\n");
        exit(0);
    }

    // handling SIGTERM signal
    struct sigaction sa;
    sa.sa_handler = sigterm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGTERM, &sa, NULL) == -1)
    {
        printf("Failed to add SIGTERM handler.\n");
        exit(0);
    }
}


int main(int argc, char const *argv[])
{
    return 0;
}
