#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "process.h"

// Inserts to the list.
void insert(struct node **head, Process *newProcess)
{
    // allocating memeory for the new node.
    struct node *newNode = malloc(sizeof(struct node));

    (*newNode).next = NULL;          // set the next pointer to NULL.
    (*newNode).process = newProcess; // Initializing the new node with the data given.

    if (!head)
    {
        return; // invalid head pointer.
    }
    else if (newNode == NULL) // checks if the memory allocation for the new node failed.
    {
        exit(1); // This system call exits the program with an error code.
    }
    else if (*head == NULL) // make the new node the head if the linked list is empty.
    {
        *head = newNode;
        return;
    }
    else // Otherwise, find the last node and append the new node to it.
    {

        struct node *current;

        for (current = *head; (*current).next != NULL; current = (*current).next)
            ;

        (*current).next = newNode;
    }

    return;
}

void delete(struct node **head, Process *process)
{
    struct node *temp = *head;
    struct node *prev = NULL;

    // special case - beginning of list
    if (temp != NULL && (*process).pid == (*(*temp).process).pid)
    {
        *head = (*temp).next;
        free(temp);
        return;
    }

    // Traverse the list to find the node with the matching process->pid
    while (temp != NULL && process->pid != temp->process->pid)
    {
        prev = temp;
        temp = temp->next;
    }

    // If the matching node is found, delete it
    if (temp != NULL)
    {
        prev->next = temp->next;
        free(temp);
    }
}

// traverse the list
void traverse(struct node *head)
{
    struct node *temp;
    temp = head;

    while (temp != NULL)
    {
        printf("[%d] [%d]\n", temp->process->pid, temp->process->cpu_time);
        temp = temp->next;
    }
}
