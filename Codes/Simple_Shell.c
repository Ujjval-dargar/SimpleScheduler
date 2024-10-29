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


// add history - command, pid, start and end time
void addHistory(const char *str)
{
    // stores last 100 previous command records
    if (sz_history < 100)
    {
        // assigning
        if (submit)
        {
            history[sz_history].pID = id;
            history[sz_history].command = strdup(str);
            history[sz_history].submit = true;
        }
        else if (shd)
        {
            history[sz_history].command = strdup(str);
            history[sz_history].schedule = true;
            history[sz_history].submit = false;
        }
        else
        {
            history[sz_history].command = strdup(str);
            history[sz_history].start = sTime;
            history[sz_history].end = eTime;
            history[sz_history].pID = id;
            history[sz_history].submit = false;
        }

        ++sz_history;
    }
    else if (sz_history == 100)
    {
        // shifting up to remove the oldest one making space for last run command
        memmove(history, history + 1, (99 * sizeof(history[0])));

        // assigning
        if (submit)
        {
            history[99].command = strdup(str);
            history[99].submit = true;
        }
        else
        {
            history[99].command = strdup(str);
            history[99].start = sTime;
            history[99].end = eTime;
            history[99].pID = id;
            history[99].submit = false;
        }
    }
}


// show previous executed commands
void showCommands()
{
    printf("\n");
    for (int i = 0; i < sz_history; ++i)
    {
        printf("\t%d. %s\n", i + 1, history[i].command);
    }
}

// Signal handler to handle SIGCHLD
void sigchld_handler(int sigNo)
{
    int status;

    while (waitpid(-1, &status, WNOHANG) > 0){}

}


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

// handling signals
void handle_signals()
{
    // handling SIGINT signal
    struct sigaction sig1;
    memset(&sig1, 0, sizeof(sig1));
    sig1.sa_handler = sigint_handler;

    if (sigaction(SIGINT, &sig1, NULL) != 0)
    {
        printf("Failed to handle signal.\n");
        exit(0);
    }

    // handling SIGCHLD signal
    struct sigaction sig2;
    sig2.sa_handler = sigchld_handler;
    sigemptyset(&sig2.sa_mask);
    sig2.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sig2, NULL) == -1)
    {
        printf("Failed to handle signal.\n");
        exit(0);
    }
}

// initialize a named pipe
void create_pipe()
{
    if (mkfifo(pipeName, 0666) == -1 && errno != EEXIST)
    {
        printf("Failed to create FIFO.\n");
        exit(0);
    }
}



// remove trailing whitespaces from start and end of str
void strip(char *str)
{
    char *start = str;
    char *end = str + strlen(str) - 1;

    while (start < end && isspace(*start))
    {
        start++;
    }

    while (end > start && isspace(*end))
    {
        end--;
    }

    memmove(str, start, end - start + 1);

    str[end - start + 1] = '\0';
}

// split str with delimiter char
char **split(const char *str, const char *dem)
{
    // copy of str to prevent changes in original
    char *copy = strdup(str);

    char **arr = NULL;
    char *ele = strtok(copy, dem); // each split element
    int i = 0;

    while (ele != NULL)
    {
        // reallocated and increase size for next element
        char **temp = realloc(arr, sizeof(char *) * (i + 1));

        checkAllocation(temp);

        arr = temp;

        arr[i] = strdup(ele);
        i++;
        ele = strtok(NULL, dem);
    }

    // NULL terminated string
    char **temp = realloc(arr, sizeof(char *) * (i + 1));

    checkAllocation(temp);

    arr = temp;
    arr[i] = NULL;

    return arr;
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



// execute command without pipe
void execute(const char *command)
{
    char *commandCopy = strdup(command);

    strip(commandCopy);
    bool background = checkbackground(commandCopy); // check for &
    if (background)
    {
        commandCopy[strlen(commandCopy) - 1] = '\0'; // removing & if present
    }
    char **args = split(commandCopy, " ");

    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            printf("Failed to cd.\n");
        }
        else if (chdir(args[1]) != 0)
        {
            printf("Failed to cd.\n");
        }
        return;
    }

    int pid = fork(); // forking a child
    id = pid;

    if (pid < 0)
    {
        printf("Failed to fork child.\n");
        exit(0);
    }
    else if (pid == 0) // child process
    {
        execvp(args[0], args);
        printf("Failed to execute.\n");
        exit(0);
    }
    else
    {
        // parent process
        if (!background) // if not background, parent will wait
        {
            int ret;
            int pid = wait(&ret);

            if (!WIFEXITED(ret)) // check terminated normally or not
            {
                printf("\nAbnormal termination with pid :%d\n", pid);
            }
        }
        return;
    }
}


// execute command with pipe
void executePipe(const char *str)
{
    char **commands = split(str, "|");
    int sz = 0;

    while (commands[sz] != NULL) // calculating number of process
    {
        sz++;
    }

    int pipefds[2 * (sz - 1)]; // for storing pipes = 2 * number of '|'
    for (int i = 0; i < sz - 1; i++)
    {
        if (pipe(pipefds + i * 2) == -1) // creating pipe and checking pipe created successfully or not
        {
            printf("Failed to create pipe.\n");
            exit(0);
        }
    }

    // creating child processes for each process
    for (int i = 0; i < sz; i++)
    {
        int pid = fork();

        if (pid < 0)
        {
            printf("Failed to fork child.\n");
            exit(0);
        }
        else if (pid == 0) // child process
        {
            if (i > 0)
            {
                dup2(pipefds[(i - 1) * 2], STDIN_FILENO); // Redirecting stdin to pipe read end
            }
            if (i < sz - 1)
            {
                dup2(pipefds[i * 2 + 1], STDOUT_FILENO); // Redirect stdout to pipe write end
            }

            // closing pipes
            for (int j = 0; j < 2 * (sz - 1); j++)
            {
                close(pipefds[j]);
            }

            strip(commands[i]);

            char **args = split(commands[i], " ");

            execvp(args[0], args);

            printf("Failed to execute.\n");

            exit(0);
        }
    }

    // closing pipes in parent
    for (int i = 0; i < 2 * (sz - 1); i++)
    {
        close(pipefds[i]);
    }

    // waiting for each child process
    for (int i = 0; i < sz; i++)
    {
        int ret;
        int pid = wait(&ret);

        if (!WIFEXITED(ret)) // check terminated normally or not
        {
            printf("\nAbnormal termination with pid :%d\n", pid);
        }
    }
}

// execute scheduler
void executeScheduler()
{
    char *ncpu_str = malloc(sizeof(char) * 10);
    char *tslice_str = malloc(sizeof(char) * 10);

    if (ncpu_str == NULL || tslice_str == NULL)
    {
        printf("Failed to malloc.\n");
        exit(0);
    }

    snprintf(ncpu_str, sizeof(char) * 10, "%d", ncpu);
    snprintf(tslice_str, sizeof(char) * 10, "%d", tslice);

    int pid = fork();
    id = pid;

    if (pid < 0)
    {
        printf("Failed to fork child.\n");
        exit(0);
    }
    else if (pid == 0)
    {
        char *args[] = {"./Simple_Scheduler", ncpu_str, tslice_str, NULL};

        execvp(args[0], args);

        printf("Failed to run scheduler.\n");
        exit(0);
    }
    else
    {
        schedulerPID = pid;
    }
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



// execute and SIGSTOP & sending its pID to scheduler
void queueExecution(char *str)
{
    char **args = split(str, " ");
    args = args + 1;

    int priority = 1;

    int sz = 0;
    while (args[sz] != NULL)
    {
        sz++;
    }

    if (sz == 2)
    {
        priority = atoi(args[1]);
        args[1] = NULL;
    }

    int pid = fork();
    if (pid < 0)
    {
        printf("Failed to fork child.\n");
        exit(0);
    }
    else
    {
        if (pid == 0)
        {
            execvp(args[0], args);
            printf("Failed to Execute.\n");
            exit(0);
        }
        else
        {
            sendToScheduler(pid, str, priority);
        }
    }
}


int main(const int argc, char const *argv[])
{

    return 0;
}
