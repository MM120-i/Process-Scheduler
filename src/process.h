/**
 *  process.h
 *
 *  Full Name: Mahim Marufuzzaman
 *  Course section: Section B
 *  Representation of a process in the system.
 *
 */

#ifndef PROCESS_H
#define PROCESS_H

// representation of a process
typedef struct process
{
    int pid;          // Unique process ID
    int cpu_time;     // CPU time required for execution
    int io_time;      // I/O time required during execution
    int arrival_time; // Time at which the process enters the system
} Process;

#endif
