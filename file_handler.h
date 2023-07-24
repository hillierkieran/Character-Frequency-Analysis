/**
 * @file    file_handler.h
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * 
 * @brief   Header for file handling utilities.
 *
 * @details Provides functions to count and retrieve the names of files in a directory.
 */

int get_n_files(char *dir_name);
char** get_files(char *dir_name, int n_files);
void free_files(char **files, int n_files);