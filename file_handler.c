/**
 * @file    file_handler.c
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * 
 * @brief   Handles file-related operations for the char_count program.
 *
 * @details This file contains functions to get the number of files in a 
 *          directory and to get the names of the files. These functions are
 *          used by the main char_count program to determine the text files to
 *          process. It includes additional error checks to ensure that 
 *          operations on directories are successful.
 */

#include <dirent.h> /* for directories      */
#include <stdio.h>  /* for fprintf          */
#include <stdlib.h>
#include <string.h>     /* for strcmp()         */
#include "file_handler.h"

#define THIS_DIR "."
#define PARENT_DIR ".."

/**
 * @brief   Count the number of files in a directory.
 *
 * @details This function opens the specified directory, and counts 
 *          the number of entries, excluding '.' and '..'.
 *
 * @param   dir_name is a string representing the directory path.
 *
 * @return  The number of files in the directory, or -1 if the 
 *          directory could not be opened.
 */
int get_n_files(char *dir_name) 
{
    int n_files = 0;
    DIR *dir_stream;
    struct dirent *entry;
    /* open the directory stream */
    if ((dir_stream = opendir(dir_name)) == NULL ) {
        fprintf(stderr, "ERROR: Failed to open directory %s\n", dir_name);
        closedir(dir_stream);
        return -1;
    }
    /* count number of files in directory  */
    while ((entry = readdir(dir_stream)) != NULL) {
        if (!strcmp(entry->d_name, THIS_DIR) || 
            !strcmp(entry->d_name, PARENT_DIR))
            continue;
        n_files++;
    }
    /*  close directory stream */
    if(closedir(dir_stream) == -1) {
        fprintf(stderr, "ERROR: Failed to close directory stream.\n");
    }

    return n_files;
}

/**
 * @brief   Retrieve the names of files in a directory.
 *
 * @details This function opens the specified directory, and stores
 *          the names of entries in a dynamically allocated array of strings,
 *          excluding '.' and '..'.
 *
 * @param   dir_name is a string representing the directory path.
 * @param   n_files is the number of files in the directory.
 *
 * @return  An array of strings where each string is a file name, or NULL if an 
 *          error occurred.
 */
char** get_files(char *dir_name, int n_files)
{
    char **files = malloc(sizeof (char *) * n_files);
    if (!files) {
        fprintf(stderr, "ERROR: Failed to allocate memory for file list\n");
        return NULL;
    }
    DIR *dir_stream;
    struct dirent *entry;
    /* open the directory stream */
    if ((dir_stream = opendir(dir_name)) == NULL ) {
        fprintf(stderr, "ERROR: Failed to open directory");
        free(files);
        return NULL;
    }
    /* add names of files in directory to list  */
    int i = 0; 
    while((entry = readdir(dir_stream)) != NULL) {
        if (!strcmp(entry->d_name, THIS_DIR) || 
            !strcmp(entry->d_name, PARENT_DIR))
            continue;
        /* get current file's name*/
        files[i] = strndup(entry->d_name, strlen(entry->d_name));
        if (!files[i]) {
            fprintf(stderr, "ERROR: Failed to get directory entry");
            free_files(files, i);
            return NULL;
        }
        i++;
    }
    /*  close directory stream */
    if(closedir(dir_stream) == -1) {
        fprintf(stderr, "ERROR: Failed to close directory stream.\n");
    }

    return files;
}

/**
 * @brief   Free a dynamically allocated array of strings.
 *
 * @details This function frees the memory allocated to the array of strings 
 *          as well as the strings themselves.
 *
 * @param   files is the array of strings to free.
 * @param   n_files is the number of strings in the array.
 */
void free_files(char **files, int n_files)
{
    for (int i = 0; i < n_files; i++)
        free(files[i]);
    free(files);
}