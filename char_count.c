/* char_count.c */
#include <math.h>       /* for floor()          */
#include <stdio.h>      /* for fprintf()        */
#include <stdlib.h>     /* for exit()           */
#include <string.h>     /* for strcmp()         */
#include <sys/wait.h>   /* for wait()           */
#include "make_ring.h"  /* for ring building    */
#include "get_files.h"  /* for getting files    */

#define BUFFSIZE 50

int parse_args(int argc, char* argv[], int* n_procs, char** dir_name)
{
    if ((argc != 3) || ((*n_procs = atoi(argv[1])) <= 0)) {
        /* User input incorrect */
        fprintf(stderr, "Usage: %s n_procs dirname\n", argv[0]);
        return (-1);
    };
    /* Correct input */
    *dir_name = argv[2];
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
    int     n_procs;        /* total number of processes in ring            */
    char*   dir_name;       /* sub-directory containing text files          */
    int     char_freq[26];  /* Array to tally each charater's frequency     */
    
    /* fill char freqency array with zeros */
    memset(char_freq, 0, sizeof(char_freq));

    /*  check arguments are valid */
    if (parse_args(argc, argv, &n_procs, &dir_name) < 0)
        exit(EXIT_FAILURE);
    
    /*  retrieve a list of files in user-defined sub-directory */
    /*  get number of files in target directory */
    size_t  n_files = get_n_files(dir_name);
    if (!n_files) {
        fprintf(stderr, "ERROR: Unable count number of files in given directory!\n");
        exit(EXIT_FAILURE);
    }
    /*  get names of files in target directory */
    char    **files = get_files(dir_name, n_files);
    if (!files) {
        fprintf(stderr, "ERROR: Unable to allocate files!\n");
        exit(EXIT_FAILURE);
    }

    /*  build ring structure of user-defined size */
    if (make_trivial_ring() < 0) {
        perror("Could not make trivial ring");
        exit(EXIT_FAILURE);
    };
    for (i = 1; i < n_procs; i++) {
        if (add_new_node(&child_pid) < 0) {
            perror("Could not add new node to ring");
            exit(EXIT_FAILURE);
        };
        if (child_pid)
            break;
    };
    
    /*  multi-process code  */
    if (i == 1) {
        /*  mother process */

        /*  pass empty array of frequencies into the ring (next process) */
    }
    /*  all processes */

    /*  get this process' tasks */
    int n_my_tasks;
    int* task_ids = divide_tasks(i, n_files, n_procs, &n_my_tasks);

    /*  receive array of frequencies from previous process */

    /*  use this process' tasks */
    for (int j = 0; j < n_my_tasks; j++) {
        fprintf(stderr, "%d of %d is handling %s/%s\n", 
        i, n_procs, dir_name, files[task_ids[j]]);
    }
    
    /*  add this process' counts to the totals for each character */

    /*  free this process' task memory after use */ 
    free(task_ids);

    if (i > 1) {
        /*  ring processes */

        /*  pass updated array to next process */

    } else {
        /*  mother process */

        /*  report total frequencies to terminal */
        
    }

    // parents wait for children to finish
    if (child_pid) {
        for (int j = 0; j < n_procs - 1; j++) {
            wait(NULL);
        }
    }
    
    exit(EXIT_SUCCESS);
} /*  end of main program here */