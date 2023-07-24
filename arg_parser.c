/* arg_parser.c */
#include <stdio.h>
#include <stdlib.h>
#include "arg_parser.h"

int parse_args(int argc, char* argv[], int* n_procs, char** dir_name)
{
    if (argc != 3) {
        /* Invalid number of arguments */
        fprintf(stderr, "ERROR: Incorrect number of arguments.\n");
        fprintf(stderr, "Usage: %s n_procs dirname\n", argv[0]);
        return -1;
    }
    
    *n_procs = atoi(argv[1]);
    if (*n_procs <= 0) {
        /* Invalid number of processes */
        fprintf(stderr, 
            "ERROR: Invalid number of processes. Should be greater than 0.\n");
        return -1;
    }

    /* Correct input */
    *dir_name = argv[2];
    return 0;
}