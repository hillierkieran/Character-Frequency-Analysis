/**
 * @file    data_processor.c
 * @brief   This file contains functions that are responsible for data processing
 * 
 * @details This file contains functions to calculate the frequency of characters,
 *          convert an array to a string, send and receive data between processes.
 * 
 * @author  Kieran Hillier
 * @date    July 24, 2023
 */

#include <string.h>     /* for strlen           */
#include <ctype.h>      /* for isalpha()        */
#include <unistd.h>     /* for stderr, etc      */
#include <stdlib.h>     /* for malloc()         */
#include <stdio.h>      /* for fprintf()        */
#include "data_processor.h"

#define BUFFER_SIZE 256
#define ALPHABET_SIZE 26
#define DELIMITER " "

/**
 * @brief   Count the frequency of each character in a file
 * @details This function reads characters from a file and updates the 
 *          frequency count of each character in the char_freq array.
 * 
 * @param   file_name - the name of the file to be read
 * @param   char_freq - the array of character frequency counts
 * @param   dir_name - the name of the directory where the file is located
 * 
 * @return  Returns the total number of characters counted, 
 *          -1 if an error occurs when opening, reading, or closing the file.
 */
long count_freq(char* file_name, long* char_freq, const char* dir_name)
{
    char file_path[BUFFER_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_name, file_name);

    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Failed to open file %s", file_name);
        return -1;
    }

    char c;
    long char_count = 0;
    /* for as long as we can get characters... */
    while ((c = fgetc(file)) != EOF) {
        if (isalpha(c)) {
            if((tolower(c)-'a') >= 0 && (tolower(c)-'a') < ALPHABET_SIZE) {
                char_freq[tolower(c)-'a'] += 1;
                char_count += 1;
            }
        }
    }

    if (ferror(file)) {
        fprintf(stderr, "ERROR: Failed to read from file.\n");
        fclose(file);
        return -1;
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "ERROR: Failed to close file.\n");
        return -1;
    }

    return char_count;
}

/**
 * @brief   Convert an array of longs to a string
 * @details This function takes an array of longs and converts it into a string 
 *          representation, with each number separated by a space.
 * 
 * @param   arr - the array to be converted
 * @param   length - the length of the array
 * 
 * @return  Returns a pointer to the string representation of the array,
 *          NULL if memory allocation for the string fails.
 */
char* array_to_string(long arr[], size_t length) {
    /*  calculate the length of string incl. space for each digit and a space */
    int string_len = length * (sizeof(long) * 8 + 1); 
    char* str = malloc(string_len);  // allocate memory for the string
    if (!str) {
        fprintf(stderr, "ERROR: Failed to allocate memory for string.\n");
        return NULL;
    }

    char* p = str;
    for (size_t i = 0; i < length; ++i) {
        // write to the string and advance the pointer
        int written = sprintf(p, "%ld%s", arr[i], DELIMITER);
        p += written;
    }
    // replace last space with null terminator
    if (p != str) {
        *(p - 1) = '\0';
    }
    return str;
}

/**
 * @brief   Send data to the standard output
 * @details This function writes a string representation of an array of longs to
 *          the standard output.
 * 
 * @param   buffer - where the string representation of the array is stored
 * @param   arr - the array to be sent
 * 
 * @return  Returns the number of bytes written,
 *          -1 if an error occurs when writing.
 */
int send_data(char* buffer, long arr[])
{
    int bytes, len;
    char* arr_as_string = array_to_string(arr, ALPHABET_SIZE);
    sprintf(buffer, "%s", arr_as_string);
    free(arr_as_string);
    len = strlen(buffer) + 1;
    if ((bytes = write(STDOUT_FILENO, buffer, len)) != len) {
        fprintf(stderr,
                "ERROR: Failed to write %d bytes, only sent %d bytes\n",
                len, bytes);
        return -1;
    }
    return bytes;
}

/**
 * @brief   Receive data from the standard input
 * @details This function reads a string representation of an array of longs
 *          from the standard input and updates the arr parameter with the
 *          values received.
 * 
 * @param   buffer - where the received string will be stored
 * @param   arr - the array where the received values will be stored
 * @param   error_status - a pointer to an integer that will be updated to -1 if an error occurs,
 *          0 otherwise.
 * 
 * @return  Returns 0 if the execution is successful, -1 in case of error.
 */
int receive_data(char* buffer, long arr[])
{
    ssize_t read_result = read(STDIN_FILENO, buffer, BUFFER_SIZE);
    if(read_result < 0){
        fprintf(stderr, "ERROR: Failed to read pipe data.\n");
        return -1;
    } else if (read_result == 0) {
        fprintf(stderr, 
                "ERROR: Reached end of pipe without reading any data.\n");
        return -1;
    }

    char* token = strtok(buffer, DELIMITER);

    for(int i = 0; i < ALPHABET_SIZE; i++){
        if (!token) {
            fprintf(stderr, "ERROR: Received incomplete data.\n");
            return -1;
        }

        arr[i] += atol(token);

        token = strtok(NULL, DELIMITER);
    }
    
    if (token != NULL) {
        fprintf(stderr, "ERROR: Received too much data.\n");
        return -1;
    }
    
    return 0;
}