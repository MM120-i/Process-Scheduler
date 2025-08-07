#ifndef SCHEDULER_H
#define SCHEDULER_H

#define SIZE 100

// length of a time quantum
#define QUANTUM 2

#define FCFS_ALGORITHM 0
#define RR_ALGORITHM 1
#define SJF_ALGORITHM 2

#define ready 1
#define running 2
#define blocked 3
#define finished 4

// Functions
void FCFS(Process *queue, int num_of_processes, const char *input_filename);
void RR(Process *queue, int num_of_processes, const char *input_filename);
void SJF(Process *queue, int num_of_processes, const char *input_filename);
char *strdup(const char *s);
void statistics(float CPU_U, int finishingTime, int num_of_process, FILE *output_file, int **turnAroundTime);
void timeSnapshot(int **processState, FILE *output_file, int num_of_processes, int clockCycle);
void FCFS_HELPER(Process *queue, int num_of_processes, int **processState, struct node **readyQueue, int **turnAroundTime, int clockCycle);
void RR_HELPER(Process *queue, int num_of_processes, int **processState, struct node **readyQueue, int **turnAroundTime, int *q, int clockCycle);
void SJF_HELPER(struct node **readyQueue, int **processState, Process *queue, int num_of_processes, int **turnAroundTime, int clockCycle, int *nextPID, int *remainingCPU, int *pid);

#endif // SCHEDULER_H
