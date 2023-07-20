#include <math.h>       /* for floor    */
#include <stdio.h>      /* for fprintf  */
#include <stdlib.h>     /* for exit     */
#include <unistd.h>     /* for fork     */
#include <sys/wait.h>   /* for wait     */

int parse_args(int argc, char *argv[], int *n_procs, int *n_tasks)
{
    if ((argc != 3) || 
        ((*n_procs = atoi(argv[1])) <= 0) || 
        ((*n_tasks = atoi(argv[2])) <= 0)) {
        /* User input incorrect */
        fprintf(stderr, "Usage: %s n_procs n_tasks\n", argv[0]);
        return (-1);
    };
    /* Correct input */
    return (0);
}

int* divide_tasks(int i, int n_total, int n_procs, int* n_my_tasks)
{
    // Calculate the number of tasks each process should handle
    int tasks_per_proc = n_total / n_procs;
    int remainder = n_total % n_procs;

    *n_my_tasks = tasks_per_proc + ((i - 1) < remainder ? 1 : 0);
    int first = (i - 1) * tasks_per_proc + ((i - 1) < remainder ? (i - 1) : remainder);

    // Allocate memory for the list of task IDs
    int* task_ids = (int*)malloc(*n_my_tasks * sizeof(int));

    // Assign task IDs to the list
    for (int j = 0; j < *n_my_tasks; j++) {
        task_ids[j] = first + j;
    }

    return task_ids;
}

int main(int argc, char *argv[])
{
    /* declare starting variables */
    int     i;              /* number of this process (starting with 1)     */
    int     child_pid;      /* indicates process should spawn another       */
    int     n_procs;        /* total number of processes                    */
    int     n_tasks;        /* total  number of tasks                       */

    /*  check arguments are valid */
    if (parse_args(argc, argv, &n_procs, &n_tasks) < 0)
        exit(EXIT_FAILURE);
    
    /*  fork user-defined number of processes */
    for (i = 1; i < n_procs; i++) {
        child_pid = fork();
        if (child_pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } 
        if (child_pid)
            break;
    }
    
    /* get this process' tasks */
    int n_my_tasks;
    int* task_ids = divide_tasks(i, n_tasks, n_procs, &n_my_tasks);

    /* use this process' tasks */
    for (int j = 0; j < n_my_tasks; j++) {
        fprintf(stderr, "Process %d of %d is handling task %d\n", i, n_procs, task_ids[j]);
    }
    
    /* free this process' task memory after use */ 
    free(task_ids);

    // parents wait for its children to finish
    if (child_pid) {
        for (int j = 0; j < n_procs - 1; j++) {
            wait(NULL);
        }
    }
    exit(EXIT_SUCCESS);
} /*  end of main program here */