/* char_count.c */
#include <ctype.h>      /* */
#include <math.h>
#include <stdio.h>      /* for fprintf()        */
#include <stdlib.h>     /* for exit()           */
#include <string.h>     /* for strcmp()         */
#include <sys/wait.h>   /* for wait()           */
#include "arg_parser.h"
#include "make_ring.h"  /* for ring building    */
#include "get_files.h"  /* for getting files    */
#include "task_manager.h"
#include "data_processor.h"
#include "result_printer.h"

#define BUFF_SIZE 256
#define ALPHABET_SIZE 26
#define BARLENGTH 65
#define DELIMITER " "

int main(int argc, char *argv[])
{
    /* declare starting variables */
    int     i;              /* number of this process (starting with 1)     */
    int     child_pid;      /* indicates process should spawn another       */
    int     n_procs;        /* total number of processes in ring            */
    char    buff[BUFF_SIZE];/* string for integers                          */
    char*   dir_name;       /* sub-directory containing text files          */
    long    char_freq[ALPHABET_SIZE]; /* Array to count charater frequencies*/
    
    /* set the frequency array to all zeros */
    memset(char_freq, 0, sizeof(char_freq));

    /*  check arguments are valid */
    if (parse_args(argc, argv, &n_procs, &dir_name) < 0)
        exit(EXIT_FAILURE);
    
    /*  retrieve a list of files in user-defined sub-directory */
    /*  get number of files in target directory */
    int n_files = get_n_files(dir_name);
    if (!n_files) {
        fprintf(stderr, 
                "ERROR: Failed to count files in the given directory!\n");
        exit(EXIT_FAILURE);
    }
    /*  get names of files in target directory */
    char** files = get_files(dir_name, n_files);
    if (!files) {
        fprintf(stderr, "ERROR: Unable to allocate files!\n");
        exit(EXIT_FAILURE);
    }

    /*  build ring structure of user-defined size */
    if (make_trivial_ring() < 0) {
        fprintf(stderr, "ERROR: Could not make a trivial ring");
        exit(EXIT_FAILURE);
    }
    for (i = 1; i < n_procs; i++) {
        if (add_new_node(&child_pid) < 0) {
            fprintf(stderr, "ERROR: Could not add a new node to the ring");
            exit(EXIT_FAILURE);
        }
        if (child_pid)
            break;
    }
    
    /*  multi-process code  */

    /*  mother process */
    if (i == 1) {
        /* send empty frequency array into ring */
        send_data(buff, char_freq);
    }

    /*  all processes */
    /*  get this process' assigned tasks */
    int my_n_tasks;
    int* task_ids = divide_tasks(i, n_files, n_procs, &my_n_tasks);

    /*  loop this process' tasks */
    for (int t = 0; t < my_n_tasks; t++) {
        snprintf(buff, BUFF_SIZE, "%s", files[task_ids[t]]);
        count_freq(files[task_ids[t]], char_freq, dir_name);
    }
    /*  free this process' task memory after use */ 
    free(task_ids);


    receive_data(buff, char_freq);

    if (i > 1) {
        /*  ring processes */
        send_data(buff, char_freq);
    } else {
        /*  mother process */
        /*  report total frequencies to terminal */
        print_result(n_procs, char_freq);
    }

    /* Wait for all processes to finish */
    if (child_pid) {
        for (int j = 0; j < n_procs - 1; j++) {
            wait(NULL);
        }
    }
    
    exit(EXIT_SUCCESS);
}