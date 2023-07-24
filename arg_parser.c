/**
 * @file    arg_parser.c
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * @brief   This file provides the definition of parse_args() function.
 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include "arg_parser.h"

/**
 * @brief Parses the command line arguments.
 *
 * @details This function checks the number of arguments and their validity. 
 *          If the arguments are not valid, it prints an error message and 
 *          returns -1.
 *
 * @param argc The count of arguments provided in the command line.
 * @param argv The array of arguments provided in the command line.
 * @param n_procs A pointer to the number of processes to be used.
 * @param dir_name A pointer to the directory name where the text files are located.
 *
 * @return Returns 0 on success and -1 on error.
 */
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

    /* Check if the directory exists */
    DIR* dir = opendir(argv[2]);
    if (dir) {
        /* Directory exists, proceed to close */
        closedir(dir);
    } else {
        /* Directory does not exist or cannot be accessed */
        fprintf(stderr, "ERROR: Directory does not exist or cannot be accessed.\n");
        return -1;
    }

    /* Correct input */
    *dir_name = argv[2];
    return 0;
}