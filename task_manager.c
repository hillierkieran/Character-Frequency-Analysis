/* task_manager.c */
#include <stdio.h>      /* for fprintf()        */
#include <stdlib.h>     /* for exit()           */
#include <string.h>     /* for strcmp()         */
#include "task_manager.h"

int* divide_tasks(int i, int n_total, int n_procs, int* my_n_tasks)
{
    // Calculate the number of tasks each process should handle
    int tasks_per_proc = n_total / n_procs;
    int remainder = n_total % n_procs;

    *my_n_tasks = tasks_per_proc + ((i - 1) < remainder ? 1 : 0);
    int first = ((i - 1) * tasks_per_proc 
                + ((i - 1) < remainder ? (i - 1) : remainder));

    // Allocate memory for the list of task IDs
    int* task_ids = (int*)malloc(*my_n_tasks * sizeof(int));
    if (!task_ids) {
        fprintf(stderr, "ERROR: Failed to allocate memory for task IDs.\n");
        exit(EXIT_FAILURE);
    }

    // Assign task IDs to the list
    for (int j = 0; j < *my_n_tasks; j++) {
        task_ids[j] = first + j;
    }

    return task_ids;
}