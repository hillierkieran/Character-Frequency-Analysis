/**
 * @file    char_count.c
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * 
 * @brief   A program for counting character frequencies.
 *
 * @details This program is designed to count the number of times each character 
 *          appears in a set of text files. It uses a multi-process architecture 
 *          with a ring structure for efficient parallel processing.
 *
 * Compilation: 
 *          Use the provided makefile to compile the program by running the command `make`. 
 *          It generates an executable named `char_count`.
 *          
 * Execution: 
 *          Run the program with the command `./char_count <n_procs> <directory>`, 
 *          where `<n_procs>` is the number of processes to spawn and `<directory>` is 
 *          the directory containing the text files to process.
 *          Alternatively, run the command `make run` for an example execution.
 */

#include <stdio.h>      /* for fprintf()    */
#include <stdlib.h>     /* for exit()       */
#include <string.h>     /* for memset()     */
#include <sys/wait.h>   /* for wait()       */

#include "arg_parser.h"     /* for parsing command line arguments             */
#include "ring_builder.h"   /* for building the process ring                  */
#include "file_handler.h"   /* for handling file I/O operations               */
#include "task_manager.h"   /* for managing tasks across processes            */
#include "data_processor.h" /* for data processing and communicating          */
#include "result_printer.h" /* for printing the results to terminal           */

#define BUFFER_SIZE 256
#define ALPHABET_SIZE 26
#define BARLENGTH 65
#define DELIMITER " "

/**
 * @brief   The main function to run the character count program.
 *
 * @details This function initializes the necessary variables and structures, 
 *          including parsing arguments, getting file names, creating a process
 *          ring, distributing tasks, and finally calculating and printing 
 *          character frequencies. 
 *
 * @param   argc is the count of arguments provided in command line.
 * @param   argv is the array of arguments provided in command line.
 *
 * @return  Returns EXIT_SUCCESS if the execution is successful, 
 *          otherwise returns EXIT_FAILURE.
 */
int main(int argc, char *argv[])
{
    /* declare variables */
    int     proc_id;        /* relative id of this process (starting with 1)  */
    int     child_pid;      /* indicates if this process should spawn another */
    int     n_procs;        /* total number of processes in ring              */
    char    buff[BUFFER_SIZE];/* string sending data                            */
    char*   dir_name;       /* sub-directory containing text files            */
    long    freq_count[ALPHABET_SIZE];/* array of charater frequency counts   */
    
    /*  set frequency array contents to zeros */
    memset(freq_count, 0, sizeof(freq_count));

    /*  check and get arguments */
    if (parse_args(argc, argv, &n_procs, &dir_name) < 0)
        exit(EXIT_FAILURE);
    
    /*  get files in user-defined sub-directory */
    /*  get number of files in target directory */
    int n_files = get_n_files(dir_name);
    if (!n_files) {
        fprintf(stderr, "ERROR: Failed to count files in the given directory!\n");
        exit(EXIT_FAILURE);
    }
    /*  get array of file names in target directory */
    char** files = get_files(dir_name, n_files);
    if (!files) {
        fprintf(stderr, "ERROR: Unable to allocate files!\n");
        free_files(files, n_files);
        exit(EXIT_FAILURE);
    }

    /*  build ring structure of user-defined number of processes */
    if (make_trivial_ring() < 0) {
        fprintf(stderr, "ERROR: Could not make a trivial ring");
        exit(EXIT_FAILURE);
    }
    for (proc_id = 1; proc_id < n_procs; proc_id++) {
        if (add_new_node(&child_pid) < 0) {
            fprintf(stderr, "ERROR: Could not add a new node to the ring");
            exit(EXIT_FAILURE);
        }
        if (child_pid)
            break;
    }
    
    /*  multi-process code  */

    if (proc_id == 1) {
        /*  mother process */
        /*  sends empty frequency array into ring */
        send_data(buff, freq_count);
    }

    /*  all processes */
    /*  get their allocated tasks */
    int n_tasks;
    int* task_ids = divide_tasks(proc_id, n_files, n_procs, &n_tasks);
    if (!task_ids) {
        free_files(files, n_files);
        free(task_ids);
        exit(EXIT_FAILURE);
    }
    /*  work through each of their tasks */
    for (int i = 0; i < n_tasks; i++) {
        if (count_freq(files[task_ids[i]], freq_count, dir_name) < 0) {
            free_files(files, n_files);
            free(task_ids);
            exit(EXIT_FAILURE);
        }
    }
    
    /* deallocate memory of no longer needed variables */
    free_files(files, n_files);
    free(task_ids);

    /*  waits to receive the frequency array and
        adds the incoming sub-total counts to its local counts */
    receive_data(buff, freq_count);

    if (proc_id > 1) {
        /*  ring processes */
        /*  sends updated sub-total counts to next process*/
        send_data(buff, freq_count);
    } else {
        /*  mother process */
        /*  reports total frequency counts to terminal */
        print_result(n_procs, freq_count);
    }

    /*  all processes */
    /*  wait for their children to finish */
    if (child_pid) {
        for (int j = 0; j < n_procs - 1; j++) {
            wait(NULL);
        }
    }
    
    /* execution successful, end */
    exit(EXIT_SUCCESS);
}