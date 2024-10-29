
# Simple Scheduler
## A Process Scheduler in C from Scratch

## Group ID - 53

## Table of Contents

1. [Overview](#overview)
2. [Implementation](#implementation)
3. [GitHub Repository Link](#github-repository-link--simplescheduler)
4. [Contribution](#contribution)

## Overview
- This project is a simple scheduler implemented in C. It schedule commands provided by the user in a **_priority based round-robin_** fashion.

## Implementation

### `message.h`
- Store a struct `message` that includes pID, command, priority.

### `Queue.h` & `Queue.c`
- Provide basic implementation of a queue - `enqueue`, `dequeue`, `is_empty`, `peek`, `clear`.

### `Simple_Shell.c`

- Global Variables :
    - `process` : stores user input command, its pID, starting and ending time.
    - `history` : stores last 100 previous process records.
    - `sz_history` : holds current number of process recorded.
    - `cwd` : stores current working directory.


- `checkAllocation()` : checks whether memory allocated successfully or not.
- `checkTime()` : checks whether `gettimeofday` successfully retrieved time or not.
- `checkbackground()` : checks whether command is to be executed in background or not.
- `checkPipe()` : checks the presence of '|' in command.


- `clear()` : clears the console.


- `addHistory()` : add history - command, pid, start and end time to `history`.
- `showCommands()` : shows previously executed commands.
- `printHistory()` : print history - command, pid, start-time and execution duration.
- `strip()` : removes trailing whitespaces from start and end of `str`.
- `split()` : split `str` with delimiter `char`.
- `input()` : take user input and check if it's not empty.
- `execute()` : executes command without pipe using `execvp` in child process.
- `executePipe()` : executes command with pipe using `execvp` in child process.


- `sigchld_handler()` : handler `SIGCHLD` signal to clear up terminated child processes
- `sig_int_handler()` : Signal handler to handle `SIGINT` (`ctrl+c`) and send `SIGTERM` to the scheduler process.
- `handle_signals()` : add handler to `SIGINT` and `SIGCHLD` signals.


- `create_pipe()` : initialize a named pipe using `mkfifo`.
- `executeScheduler()` : fork and execute scheduler with `ncpu` and `tslice` arguments. Also stores scheduler pId.
- `sendToScheduler()` : write to the pipe i.e. sending a message to scheduler.
- `queueExcepution()` : command with `submit` prefix are forked and its pID is sent to scheduler for execution.


- `main()` : displays a command prompt, handles `SIGINT` signal, take user inputs and executes them.

**Command with `submit` prefix are being forked and sent to scheduler using pipes. Rest command are working on the shell without any scheduling.**

### `Simple_Scheduler.c`

**SimpleScheduler use priority scheduling by allowing the user to specify a priority value
in the range 1-4 as a command line parameter while submitting the job as follows:
 `submit ./a.out 2`**


**If the priority is not specified by the user, the job will have the default priority of 1.**


**1 - Highest priority**<br>
**4 - Lowest priority**

- Global Variables :
  - `process` : stores user input command, its pID, starting and ending time.
  - `pipeName` : store the name of the fifo pipe.
  - `readyQueue` : stores ready process.
  - `runningQueue` : stores currently running process.
  - `completedQueue` : stores completed process.
  - `readyFront` & `readyRear` : stores front and rear of the ready queue.
  - `runningFront` & `runningRear` : stores front and rear of the running queue.
  - `completedFront` & `completedRear` : stores front and rear of the completed queue.
  - `readyQueueSize` : stores current size of ready process.
  - `runningQueueSize` : stores current size of running process.
  - `completedQueueSize` : store current size of completed process.


- `readyQueue_isEmpty()` : checks whether ready queue is empty or not.
- `runningQueue_isEmpty()` : checks whether running queue is empty or not.
- `completedQueue_isEmpty()` : checks whether completed queue is empty or not.


- `addWaitTime()` : add `tslice` (wait_time) to all processes in ready queue.
- `stopRunningProcesses()` : stops all running processes from running queue using `SIGSTOP` signal.
- `receiveMessage()` : receive message (process) from shell and add it to the ready queue.
- `sleepTslice()` : sleep scheduling (scheduler) for `tslice` millisecond.
- `execute()` : continue executing at most `ncpu` processes from ready queue using `SIGCONT` signal.
- `printHistory()` : print history - command, pid, arrival-time, waiting-time and execution duration of all the completed process.


- `sigterm_handler()` : handle `SIGTERM` signal to print history.
- `handle_signals()` : add handler to `SIGTERM` signals and handling `SIGINT` signal to be ignored



- `main()` : stores `ncpu` and `tslice` given in arguments. Indefinitely receive messages, execute and stop running process.

### `dummy_main.h`

We require `dummy_main.h` due to following reasons -

- To provide controlled execution of a program
- Ensures that program won’t start running immediately after it’s being forked
- It blocks until it receives a `SIGCONT` signal from scheduler
- Ignoring the `SIGINT` signal ensures that the process doesn’t terminate if interrupt occur. 
- Termination and handling of the process can be managed by scheduler


## GitHub Repository Link : [SimpleScheduler](https://github.com/Akshat-Sin/SimpleShell)


## Contribution

#### Members -
>- _Akshat Singh 2023064_
>- _Ujjval Dargar 2023564_

Both members have made equal contributions to the project.

The responsibilities were divided as follows:

### `Simple_Shell.c`

- Akshat Singh

    - Implemented `handle_signals()` function to add handler to `SIGINT` and `SIGCHLD` signal.
    - Implemented `sigchld_handler()` function to handle `SIGCHLD` signal.
    - Implemented `create_pipe()` function to initialize pipe.
    - Implemented `executeScheduler()` to execute scheduler.
    - Assisted in developing the `main()` function.
    - Assisted in developing the `executePipe()` function.
    - Implemented `checkAllocation()`, `checkTime()`, `checkbackground()`, `checkPipe()`.
    - Implemented `clear()` which clears the console.
    - Implemented `printHistory()` which is responsible for printing history - command, pid, start-time and execution duration.
    - Implemented `addCommands()` function to add command for scheduling.
    - Debugged the code to ensure it works correctly.


- Ujjval Dargar

    - Implemented `input()` function, which is responsible for input of commands.
    - Implemented `sig_int_handler()` function to handle `SIGINT` signal.
    - Implemented `sendToScheduler()` function to send message to scheduler.
    - Implemented `queueExecution()` to queue a process execution.
    - Implemented `execute()` function, which is responsible for execution of commands.
    - Implemented `executePipe()` function, which is responsible for execution of commands with pipes.
    - Implemented `split()` which is responsible for splitting of command.
    - Implemented `strip()` which is responsible for removing trailing whitespaces.
    - Implemented `addHistory()` which is responsible for add history - command, pid, start and end time.
    - Implemented `showCommands()` which is responsible for showing previous executed commands.
    - Contributed to the `main()` function and overall project integration and testing.
    - Implemented `schedule()` function to start execution of added commands.
    - Documented the implementation ensuring clear documentation in README and inline comments.

### `Simple_Scheduler.c`

- Akshat Singh

  - Implemented `receiveMessage()` function to receive message in scheduler.
  - Implemented `sleepTslice()` function to pause scheduling.
  - Implemented `printHistory()` function to print process execution details.
  - Implemented `sigterm_handler()` function to print history on receiving `SIGTERM` signal.
  - Implemented the `main()` function.
  - Debugged the code to ensure it works correctly.
  


- Ujjval Dargar

  - Implemented `readyQueue`, `runningQueue`, `completedQueue` and their respective front and rear pointers.
  - Implemented `execute()` function to continue the  execution of processes.
  - Implemented `addWaitTime()` function to increment waiting time of processes.
  - Implemented `stopRunningProcess()` function to stop execution of running process.
  - Implemented `handle_signals()` to add handler to `SIGINT` and `SIGTERM`.
  - Documented the implementation ensuring clear documentation in README and inline comments.
  

### `Queue.h` & `Queue.c` 
  - Akshat Singh

### `dummy_main.h` 
  - Ujjval Dargar


---