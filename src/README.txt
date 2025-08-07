
1)  Introduction

    Assignment 1: Scheduling 

    This program is part of an assignment for EECS 3221 (Operating System Fundamentals). 
    The goal is to simulate various process scheduling algorithms and observe how different scheduling strategies
    affect the execution time of processes. This README provides information about the included files, dependencies, how to build and run the program, and an overview of its functionality.


    Description:
    The assignment involves simulating three of the following scheduling algorithms: 
    First-Come-First-Serve (FCFS), Round Robin (RR), and Shortest Remaining Job First (SJF). 
    The program reads input data describing processes and their characteristics, applies the specified scheduling algorithm, and generates output that includes timing snapshots and statistics.
    
2) Included Files

    scheduler.c: The main program file is responsible for reading input, simulating scheduling algorithms, and generating output.

    scheduler.h: Header file defining constants and function prototypes for the scheduler.

    list.h and list.c: Files implementing a linked list data structure for managing processes.

    process.h: Header file defining the "Process" structure representing a process.

    MakeFile: Automates the compilation and builds processes of the program, making it easier to manage and correct code compilation.

3) Dependencies

    list.c: The source code file containing implementations for various list operations.

    list.h: This header file contains the declarations for the list operations implemented in the list.c file.

    MakeFile: This file is used for compiling the program.

    process.h: This header file defines the structure for representing a process.

    scheduler.c: The main source code file for the scheduling program. It includes the main, FCFS, RR, and SJF functions. The main function calls the scheduling function depending on the 
                 user input through the terminal. It also creates a text file containing the output. 

    scheduler.h: The header file containing function prototypes and declarations that are related to the scheduler.

    Generated Executable file: This file is generated after compiling the source code. 

    Text files: These are the generated files from the program. For instance, it can be 2processes-1.txt, or 3processes-2.txt etc. 

    Note: The program requires these files stated above to work. 

4) Building and Running 

   In the working directory, please type:
   make
   
   then type:
   ./scheduler inputfilename-s.txt 

   's' is the number between 0 and 2. They represent the algorithms that are numbered. 0 is FCFS, 1 is RR, and 2 is SJF.
   for instance, if you want to use the FCFS algorithm using 3processes.txt, then it would be:

    ./scheduler 3processes.txt

    A file named 3processes-0.txt will appear in the same directory, and it can be viewed by the user. 

    In case if the 'make' command is not working, then this can be typed:
    gcc -o scheduler scheduler.c list.c -std=c99 -Wall -lm
    then,
    ./scheduler 3processes.txt 0

8) Output Format
   
   My program produces an output file with the name: inputfilename-s.txt
   
   The contents of the file are the following:
   - Timing snapshot (starting from cycle 0)
   - Statistics
