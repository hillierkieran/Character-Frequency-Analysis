/**
 * @file    task_manager.c
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * 
 * @brief   Allocates tasks evenly to processes in a ring.
 * 
 * @details Each process in a ring architecture gets assigned tasks based on 
 *          the total number of tasks and the total number of processes. This 
 *          ensures a balanced workload across all processes.
 */

#include <stdio.h>      /* for fprintf()        */
#include <string.h>     /* for strcmp()         */
#include <stdlib.h>     /* for malloc()         */
#include "task_manager.h"

/**
 * @brief Divides tasks evenly across all processes in the ring.
 * 
 * @param proc_id The ID of the process.
 * @param n_total The total number of tasks.
 * @param n_procs The total number of processes.
 * @param n_tasks A pointer to the number of tasks this process should handle.
 * 
 * @return Returns a pointer to the list of task IDs this process should handle.
 *         In case of an error during memory allocation, it returns NULL.
 */
int* divide_tasks(int proc_id, int n_total, int n_procs, int* n_tasks)
{
    // Calculate the number of tasks each process should handle
    int tasks_per_proc = n_total / n_procs;
    int remainder = n_total % n_procs;

    *n_tasks = tasks_per_proc + ((proc_id - 1) < remainder ? 1 : 0);
    int first = ((proc_id - 1) * tasks_per_proc 
                + ((proc_id - 1) < remainder ? (proc_id - 1) : remainder));

    // Allocate memory for the list of task IDs
    int* task_ids = (int*)malloc(*n_tasks * sizeof(int));
    if (!task_ids) {
        fprintf(stderr, "ERROR: Failed to allocate memory for task IDs.\n");
        return NULL;
    }

    // Assign task IDs to the list
    for (int j = 0; j < *n_tasks; j++) {
        task_ids[j] = first + j;
    }

    return task_ids;
}