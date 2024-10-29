#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "message.h"

// command
char *command = NULL;

char *commands[100];
int sz = 0;

typedef struct
{
    char *command;
    int pID;
    struct timeval start;
    struct timeval end;
    bool submit;
    bool schedule;
} process;

// stores last 100 previous command records
process history[100];
int sz_history = 0;

int id;
int fd;

char cwd[1000];

struct timeval sTime;
struct timeval eTime;

int ncpu, tslice;
int schedulerPID;

bool submit = false;
bool shd = false;

char *pipeName = "/tmp/Simple_Scheduler_pipe";


// Signal handler to handle SIGINT
void sigint_handler(int sigsz)
{
    if (sigsz == SIGINT)
    {
        printHistory();

        if (kill(schedulerPID, SIGTERM) == -1)
        {
            printf("Failed to terminate scheduler\n");
        }

        int status;
        while (waitpid(schedulerPID, &status, WNOHANG) == 0){}

        exit(0);
    }
}


// input and check if it's not empty
bool input()
{
    command = (char *)malloc(100);
    if (command == NULL)
    {
        printf("Failed to allocate memory for command.\n");
        exit(0);
    }

    if (fgets(command, 100, stdin) == NULL)
    {
        printf("Failed to read input.\n");
        exit(0);
    }

    strip(command);

    return (strlen(command) != 0 && command[0] != '\n' && command[0] != ' ');
}




// sending message to scheduler
void sendToScheduler(int pid, char *str, int priority)
{
    int fd = open(pipeName, O_WRONLY);
    if (fd == -1)
    {
        printf("Failed to open pipe.\n");
        exit(0);
    }

    struct message msg;
    msg.pID = pid;
    strncpy(msg.command, str, sizeof(msg.command) - 1);
    msg.priority = priority;

    msg.command[sizeof(msg.command) - 1] = '\0';

    if (write(fd, &msg, sizeof(struct message)) == -1)
    {
        printf("Failed to write to pipe.\n");
    };

    close(fd);
}


int main(const int argc, char const *argv[])
{

    return 0;
}
