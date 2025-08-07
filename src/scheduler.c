#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "list.h"
#include "scheduler.h"
#include "process.h"

int main(int argc, char *argv[])
{
	const char *input_filename = argv[1];

	FILE *fp; // Open inputfile for reading.

	int num_of_processes; // Variable to store the number of processes.

	fp = fopen(input_filename, "r"); // Opens the input file specified in the command line argument.

	fscanf(fp, "%d", &num_of_processes); // Reads the number of processes from the input file.

	if (num_of_processes > 0) // If there are valid number of processes to simulate.
	{

		Process *queue = malloc(num_of_processes * sizeof(Process)); // Allocates memory to store process info for the simulation.

		for (int i = 0; i < num_of_processes; i++) // Reads the process info for each of them from the input file.
		{

			fscanf(fp, "%d %d %d %d",
				   &queue[i].pid,
				   &queue[i].cpu_time,
				   &queue[i].io_time,
				   &queue[i].arrival_time);
		}

		// Print the read process information (for debugging purposes).
		for (int i = 0; i < num_of_processes; i++)
		{
			printf("A: %d B: %d  C: %d D: %d \n",
				   queue[i].pid,
				   queue[i].cpu_time,
				   queue[i].io_time,
				   queue[i].arrival_time);
		}

		// Depending on the specified scheduling algorithm, it calls the corresponding function.
		switch (atoi(argv[2]))
		{
		case FCFS_ALGORITHM:

			FCFS(queue, num_of_processes, input_filename);
			break;

		case RR_ALGORITHM:

			RR(queue, num_of_processes, input_filename);
			break;

		case SJF_ALGORITHM:

			SJF(queue, num_of_processes, input_filename);
			break;

		default:

			break;
		}

		free(queue); // Free memory allocation for process info.
	}

	fclose(fp); // Closes the input file.

	return 0;
}

void FCFS(Process *queue, int num_of_processes, const char *input_filename)
{

	char *base_filename = strdup(input_filename); // Creats a copy of the input_filename by duplicating its contents into another array.
	char *dot = strrchr(base_filename, '.');	  // Locates the file extension.
	int **processState, **turnAroundTime;
	int cpuWork = 0, i, finishingTime = 0, clockCycle = 0;
	float CPU_U = 0.0;
	char output_filename[256];
	bool flag = false;
	struct node *temp, *readyQueue = NULL;

	// Allocating memory.
	processState = malloc(num_of_processes * sizeof(int *));
	turnAroundTime = malloc(num_of_processes * sizeof(int *));

	// When a dot is found, we truncate the base_filename string at the dot by replacing it with '\0'
	// Removes the file extension with the base filename.
	if (dot)
	{
		*dot = '\0';
	}
	// Stores the name of the of output file.
	snprintf(output_filename, sizeof(output_filename), "%s-%d.txt", base_filename, FCFS_ALGORITHM); // Create the output file name. We append "-0.txt" becuase this is FCFS algorithm.

	FILE *output_file = fopen(output_filename, "w"); // Opens the output file for writing.

	// Initialize process state and turnaround time arrays.
	for (i = 0; i < num_of_processes; i++)
	{
		processState[i] = malloc(2 * sizeof(int));
		turnAroundTime[i] = malloc(2 * sizeof(int));
		*(processState[i]) = 0;
		*(processState[i] + 1) = ceil(0.5 * queue[i].cpu_time);
	}

	// This is the simulation loop
	while (SIZE > clockCycle)
	{
		// Call FCFS_HELPER to handle process state transitions.
		// I have included helper function because it helps me keep things organized, and easy to maintain.
		// Otherwise the function becomes long and tedious.
		FCFS_HELPER(queue, num_of_processes, processState, &readyQueue, turnAroundTime, clockCycle);

		// Check for newly arriving processes and insert them into the ready queue.
		i = 0;
		while (num_of_processes > i)
		{
			if (queue[i].arrival_time == clockCycle)
			{
				insert(&readyQueue, &queue[i]);
				turnAroundTime[i][0] = clockCycle;
			}

			i++;
		}

		// Check if all processes are finished.
		flag = false;
		i = 0;
		while (num_of_processes > i)
		{
			flag = (processState[i][0] != finished) ? true : flag;
			i++;
		}
		// Updates the process state
		// Tracks the cpu utilization
		// Handles the termination of the simulation.
		for (temp = readyQueue; temp != NULL; temp = temp->next)
			processState[(*(*temp).process).pid][0] = (temp == readyQueue) ? 2 : 1;

		cpuWork += (readyQueue != NULL) ? 1 : 0;

		if (!flag)
		{
			finishingTime = clockCycle - 1;
			CPU_U = cpuWork / (float)clockCycle;
			break;
		}

		// Calls the timeSnapshot function to output the snapshot of the processes state at the current clock cycle.
		timeSnapshot(processState, output_file, num_of_processes, clockCycle);
		clockCycle++;
	}

	// Calls the statistics function in order to output the cpu utilization and the turnaround process.
	statistics(CPU_U, finishingTime, num_of_processes, output_file, turnAroundTime);

	// Dynamically frees the allocated memeory.
	for (i = 0; i < num_of_processes; i++)
	{
		free(processState[i]);
		free(turnAroundTime[i]);
	}

	free(processState);
	free(turnAroundTime);
	free(base_filename);
	fclose(output_file); // closes the file.
}

// Helper function for fcfs to manage process state transitions.
void FCFS_HELPER(Process *queue, int num_of_processes, int **processState, struct node **readyQueue, int **turnAroundTime, int clockCycle)
{
	for (int index = 0; num_of_processes > index; index++)
	{
		// Checks if the current process is in a blocked state.
		if (*(*(processState + index)) == blocked)
		{
			queue[index].io_time--; // If so, then the I/O time decreases.

			// If the I/O time reaches 0, then insert the process into the ready queue.
			(queue[index].io_time == 0) ? insert(readyQueue, &queue[index]) : 0;
		}
		else if (*readyQueue != NULL && (*readyQueue)->process->pid == index)
		{
			// If the current process is at the front of the ready queue
			switch (processState[index][0])
			{

			case ready:

				// Change the process state the running.
				*(*(processState + index)) = running;
				break;

			case running:

				// Decrement the cpu time of the process.
				(*(*readyQueue)->process).cpu_time--;

				// If the cpu time reaches 0, then mark the process as finished
				if ((*(*readyQueue)->process).cpu_time == 0)
				{
					processState[(*(*readyQueue)->process).pid][0] = finished;

					// Records the turnaround time for the process.
					turnAroundTime[(*(*readyQueue)->process).pid][1] = clockCycle;
					delete(readyQueue, (*readyQueue)->process); // deletes the finished process
				}
				else if ((*(*readyQueue)->process).cpu_time == processState[(*(*readyQueue)->process).pid][1])
				{ // If the CPU time equals initial burst time, mark the process as blocked and remove it from the ready queue.

					processState[(*(*readyQueue)->process).pid][0] = blocked;
					delete(readyQueue, (*readyQueue)->process);
				}

				break;

			default:
				break;
			}
		}
	}
}

// Round Robin scheduling algorithm with a quantum of 2
void RR(Process *queue, int num_of_processes, const char *input_filename)
{

	char *base_filename = strdup(input_filename); // Creats a copy of the input_filename by duplicating its contents into another array.
	char *dot = strrchr(base_filename, '.');	  // Locates the file extension.
	int **processState, **turnAroundTime;
	int finishingTime = 0, i, q = QUANTUM, cpuWork = 0, clockCycle = 0;
	float CPU_U = 0.0;
	bool flag = false;

	// When a dot is found, we truncate the base_filename string at the dot by replacing it with '\0'
	// Removes the file extension with the base filename.
	if (dot)
	{
		*dot = '\0';
	}

	// Create the output filename based on the algorithm type
	char output_filename[256];																	  // Stores the name of the of output file.
	snprintf(output_filename, sizeof(output_filename), "%s-%d.txt", base_filename, RR_ALGORITHM); // Create the output file name. We append "-1.txt" becuase this is RR algorithm.

	FILE *output_file = fopen(output_filename, "w"); // Opens the output file for writing.

	// Initialize the temp variable and the ready queue.
	struct node *readyQueue = NULL, *temp;

	// Allocate memeory for process states and the turnaround arroys.
	processState = malloc(num_of_processes * sizeof(int *));
	turnAroundTime = malloc(num_of_processes * sizeof(int *));

	for (int i = 0; i < num_of_processes; i++)
	{
		processState[i] = malloc(2 * sizeof(int));
		turnAroundTime[i] = malloc(2 * sizeof(int));
		processState[i][0] = 0;
		processState[i][1] = ceil(0.5 * queue[i].cpu_time);
	}

	// Simulation loop
	while (SIZE > clockCycle)
	{
		// Calls the RR_HELPER function to do the process transitions.
		//  I have included helper function because it helps me keep things organized, and easy to maintain.
		// Otherwise the function becomes long and tedious.
		RR_HELPER(queue, num_of_processes, processState, &readyQueue, turnAroundTime, &q, clockCycle);

		i = 0;
		// Checks for processes arriving at this clock cycle and inserts them into the ready queue.
		while (num_of_processes > i)
		{
			if (queue[i].arrival_time == clockCycle)
			{
				insert(&readyQueue, &queue[i]);
				turnAroundTime[i][0] = clockCycle;
			}

			flag = (processState[i][0] != finished) ? true : flag;
			i++;
		}

		flag = false;
		i = 0;
		while (num_of_processes > i)
		{
			flag = (processState[i][0] != finished) ? true : flag;
			i++;
		}

		// Updates process states in the ready queue and manage the quantum.
		for (temp = readyQueue; temp != NULL; temp = temp->next)
		{
			if (temp != readyQueue)
			{
				processState[temp->process->pid][0] = ready;
			}
			else
			{
				processState[temp->process->pid][0] = running;
				q = (q == 0) ? QUANTUM : q;
			}
		}

		// increment the cpu work counter as the cpu is doing work
		cpuWork += (readyQueue != NULL) ? ready : 0;

		if (!flag)
		{
			// Record the finishing time and cpu utilization
			finishingTime = clockCycle - 1;
			CPU_U = cpuWork / (float)(clockCycle);
			break;
		}

		// Calls the timeSnapshot function to output the snapshot of the processes state at the current clock cycle.
		timeSnapshot(processState, output_file, num_of_processes, clockCycle);
		clockCycle++;
	}

	// Calls the statistics function in order to output the cpu utilization and the turnaround process.
	statistics(CPU_U, finishingTime, num_of_processes, output_file, turnAroundTime);

	// Dynamically frees the allocated memeory.
	for (i = 0; i < num_of_processes; i++)
	{
		free(processState[i]);
		free(turnAroundTime[i]);
	}

	free(processState);
	free(turnAroundTime);
	free(base_filename);
	fclose(output_file); // closes the file.
}

// Helper function for RR to manage proces state transitions.
void RR_HELPER(Process *queue, int num_of_processes, int **processState, struct node **readyQueue, int **turnAroundTime, int *q, int clockCycle)
{

	// Iterate through all of the processes in the queue.
	for (int index = 0; num_of_processes > index; index++)
	{

		// If a process is in a blocked state, then decrement its I/O time
		if (*(*(processState + index)) == blocked)
		{

			queue[index].io_time--;

			// If I/O time reaches 0, then insert the process into the ready queue.
			(queue[index].io_time == 0) ? insert(readyQueue, &queue[index]) : 0;
		}
		else if (*readyQueue != NULL && (*readyQueue)->process->pid == index)
		{

			switch (processState[index][0])
			{

			case ready:

				// If a process is in a ready state, then change its state to running and reset the quantum.
				*(*(processState + index)) = running;
				*q = QUANTUM;
				break;

			case running:

				// If a process is in a running state, decrement its cpu time and the quantum.
				(*(*readyQueue)->process).cpu_time--;
				(*q)--;

				// If the cpu time reaches 0, then mark the process as finsihed and save the turnaround time.
				if ((*(*readyQueue)->process).cpu_time == 0)
				{

					processState[(*(*readyQueue)->process).pid][0] = finished;
					turnAroundTime[(*(*readyQueue)->process).pid][1] = clockCycle;
					delete(readyQueue, (*readyQueue)->process);
				}
				else if ((*(*readyQueue)->process).cpu_time == processState[(*(*readyQueue)->process).pid][1])
				{

					// If the cpu time equals the initial burst time, then mark the process as blocked and delete the process from the queue.
					processState[(*readyQueue)->process->pid][0] = blocked;
					delete(readyQueue, (*readyQueue)->process);
				}
				else if (*q == 0) // when quantum reaches 0...
				{

					// if the process has no cpu time left, then mark it as finished and save the turnaround time.
					if (0 >= (*(*readyQueue)->process).cpu_time)
					{

						turnAroundTime[(*(*readyQueue)->process).pid][1] = clockCycle;
						processState[(*(*readyQueue)->process).pid][0] = finished;
					} // otherwise if the cpu time equals the inital burst time, mark the process as blocked.
					else if ((*(*readyQueue)->process).cpu_time == processState[(*(*readyQueue)->process).pid][1])
					{
						processState[(*(*readyQueue)->process).pid][0] = blocked;
					}
					else // If the process stll have cpu time left, then mark it as ready and insert it back to the ready queue.
					{

						processState[(*(*readyQueue)->process).pid][0] = ready;
						insert(readyQueue, &queue[index]);
					}

					delete(readyQueue, (*readyQueue)->process);
					*q = QUANTUM; // Reset the quantum for the current process
				}

				break;

			default:
				break;
			}
		}
	}
}

void SJF(Process *queue, int num_of_processes, const char *input_filename)
{

	char *base_filename = strdup(input_filename); // Creats a copy of the input_filename by duplicating its contents into another array.
	char *dot = strrchr(base_filename, '.');	  // Locates the file extension.
	int **processState, **turnAroundTime;
	int cpuWork = 0, i, remainingCPU = SIZE, nextPID = -3, finishingTime = 0, clockCycle = 0, pid = 0;
	float CPU_U = 0.0;
	char output_filename[256];
	bool flag = false;
	struct node *readyQueue = NULL;

	// Allocating memory.
	processState = malloc(num_of_processes * sizeof(int *));
	turnAroundTime = malloc(num_of_processes * sizeof(int *));

	// When a dot is found, we truncate the base_filename string at the dot by replacing it with '\0'
	// Removes the file extension with the base filename.
	if (dot)
	{
		*dot = '\0';
	}

	// Stores the name of the of output file.
	snprintf(output_filename, sizeof(output_filename), "%s-%d.txt", base_filename, SJF_ALGORITHM);

	FILE *output_file = fopen(output_filename, "w"); // Opens the output file for writing.

	// Initialize process state and turnaround time arrays.
	for (i = 0; i < num_of_processes; i++)
	{
		processState[i] = malloc(2 * sizeof(int));
		turnAroundTime[i] = malloc(2 * sizeof(int));
		*(processState[i]) = 0;
		*(processState[i] + 1) = ceil(0.5 * queue[i].cpu_time);
	}
	// This is the simulation loop
	while (SIZE > clockCycle)
	{

		// Handles blocked processes in the ready queue.
		for (int index = 0; index < num_of_processes; index++)
		{
			if (*(*(processState + index)) == blocked)
			{
				queue[index].io_time--;

				if (queue[index].io_time == 0)
				{
					*(*(processState + index)) = ready;
					insert(&readyQueue, &queue[index]);
				}
			}
		}

		// Call FCFS_HELPER to handle process state transitions.
		// I have included helper function because it helps me keep things organized, and easy to maintain.
		// Otherwise the function becomes long and tedious.
		SJF_HELPER(&readyQueue, processState, queue, num_of_processes, turnAroundTime, clockCycle, &nextPID, &remainingCPU, &pid);

		nextPID = -3;
		i = 0;
		remainingCPU = SIZE;

		// Determine the next process to execute based on the arriavl time
		while (num_of_processes > i)
		{
			if (queue[i].arrival_time == clockCycle)
			{
				turnAroundTime[i][0] = clockCycle;
				remainingCPU = (remainingCPU > queue[i].cpu_time) ? queue[i].cpu_time : remainingCPU;
				nextPID = (remainingCPU == queue[i].cpu_time) ? queue[i].pid : nextPID;
			}

			i++;
		}

		// Handle process scheduling.
		if (nextPID != -3)
		{
			// If there are processes in the ready queue, then check for processes that arrives
			//  at the current clock cycle, and add them to the ready queue.
			if (readyQueue != NULL)
			{

				i = 0;
				while (num_of_processes > i)
				{

					if ((*(queue + i)).arrival_time == clockCycle)
					{
						// Insert in the queue
						insert(&readyQueue, &queue[i]);
						processState[i][0] = ready;
					}

					i++;
				}
			}
			else // If the ready queue is empty, then insert the process with the shortest remaining time
			{
				// inserts
				insert(&readyQueue, &queue[nextPID]);
				processState[nextPID][0] = running; // Marks the state as ready.

				i = 0;
				while (num_of_processes > i)
				{
					// If other processes arrive at the clock cycle, then insert them in the ready queue.
					if ((*(queue + i)).arrival_time == clockCycle)
					{
						if ((*(queue + i)).pid == nextPID)
						{
							break; // break the loop.
						}
						else
						{
							insert(&readyQueue, &queue[i]);
							processState[i][0] = ready;
						}
					}

					i++;
				}
			}
		}

		i = 0;
		flag = false;

		// Checks if all processes have finsihed execution.
		while (num_of_processes > i)
		{

			flag = (processState[i][0] != finished) ? true : flag;
			i++;
		}

		// increment the cpu work counter as the cpu is doing work
		cpuWork += (readyQueue != NULL) ? 1 : 0;

		if (!flag)
		{

			finishingTime = clockCycle - 1;
			CPU_U = cpuWork / (float)clockCycle;
			break;
		}
		// Calls the timeSnapshot function to output the snapshot of the processes state at the current clock cycle.
		timeSnapshot(processState, output_file, num_of_processes, clockCycle);
		clockCycle++;
	}

	// Calls the statistics function in order to output the cpu utilization and the turnaround process.
	statistics(CPU_U, finishingTime, num_of_processes, output_file, turnAroundTime);

	// Dynamically frees the allocated memeory.
	for (i = 0; i < num_of_processes; i++)
	{
		free(processState[i]);
		free(turnAroundTime[i]);
	}

	free(processState);
	free(turnAroundTime);
	free(base_filename);
	fclose(output_file); // closes the file.
}

// SJF helper function.
void SJF_HELPER(struct node **readyQueue, int **processState, Process *queue, int num_of_processes, int **turnAroundTime, int clockCycle, int *nextPID, int *remainingCPU, int *pid)
{
	struct node *temp;
	Process *process;
	int i = 0;

	// If there is a running process in the queue...
	if (*readyQueue != NULL && processState[(*(*(*readyQueue)).process).pid][0] == running)
	{
		process = (*(*readyQueue)).process;
		*pid = (*process).pid;

		(*process).cpu_time--; // decrease the cpu time

		// If the running process has completed its cpu burst.
		if (process->cpu_time == 0)
		{
			processState[*pid][0] = finished;
			turnAroundTime[*pid][1] = clockCycle;
		}
		else if ((*process).cpu_time == processState[*pid][1]) // if the running processes need to be blocked for I/O
		{
			processState[*pid][0] = blocked;
		}

		// If the running process has completed its execution.
		if ((*process).cpu_time == processState[*pid][1] || (*process).cpu_time == 0)
		{
			// Delete the process from the ready queue.
			delete(readyQueue, &queue[*pid]);
		}
	}

	// If there are ready processes in the queue
	if (*readyQueue != NULL && processState[(*(*(*readyQueue)).process).pid][0] == ready)
	{
		*nextPID = -3;
		i = 0;
		*remainingCPU = SIZE;

		// iterates through the queue...
		// And locates the shortest remaining cpu time in the queue.
		for (temp = *readyQueue; temp != NULL; temp = (*temp).next)
		{
			if (*remainingCPU > (*(*temp).process).cpu_time)
			{
				*remainingCPU = (*(*temp).process).cpu_time;
				*nextPID = (*(*temp).process).pid;
			}
		}

		// Marks the process as running.
		processState[*nextPID][0] = running;

		// Deletes the other processes from the ready queue
		while (num_of_processes > i)
		{
			delete(readyQueue, &queue[i]);
			i++;
		}

		// Inserts in the ready queue.
		insert(readyQueue, &queue[*nextPID]);

		i = 0;
		while (num_of_processes > i)
		{
			// Inserts the ready process into the ready queue.
			(*nextPID != i && processState[i][0] == ready) ? insert(readyQueue, &queue[i]) : 0;
			i++;
		}
	}
}

// Function to print the statistics.
void statistics(float CPU_U, int finishingTime, int num_of_processes, FILE *output_file, int **turnAroundTime)
{

	int i = 0;

	// prints onto the output file:
	fprintf(output_file, "\nFinishing time: %d\n", finishingTime);
	fprintf(output_file, "CPU utilization: %0.2f\n", CPU_U);

	// Calculates the tunraround time for each process.
	while (i < num_of_processes)
	{
		// end time - arrival time
		fprintf(output_file, "Turnaround process %d: %d\n", i, turnAroundTime[i][1] - turnAroundTime[i][0]);
		i++;
	}
}

// Function to print the time snapshot.
void timeSnapshot(int **processState, FILE *output_file, int num_of_processes, int clockCycle)
{
	int i = 0;

	// prints onto the output file the current clock cycle.
	fprintf(output_file, "%d ", clockCycle);

	// Loops through each process and prints the states.
	while (i < num_of_processes)
	{

		switch (processState[i][0]) // checks the state.
		{

		case ready:

			fprintf(output_file, "%d: ready ", i);
			break;

		case running:

			fprintf(output_file, "%d: running ", i);
			break;

		case blocked:

			fprintf(output_file, "%d: blocked ", i);
			break;

		default:
			break;
		}

		i++;
	}

	// prints a new line onto the output file.
	fprintf(output_file, "\n");
}
