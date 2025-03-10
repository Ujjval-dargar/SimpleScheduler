
# Simple Scheduler
## A Process Scheduler in C from Scratch

## Table of Contents

1. [Overview](#overview)
2. [GitHub Repository Link](#github-repository-link--simplescheduler)

## Overview
- This project is a simple scheduler implemented in C. It schedule commands provided by the user in a **_priority based round-robin_** fashion.


## GitHub Repository Link : [SimpleScheduler](https://github.com/Ujjval-dargar/SimpleScheduler)


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
