/* data_processor.c */
#include <string.h>     /* for strlen           */
#include <ctype.h>      /* for isalpha()        */
#include <unistd.h>     /* for stderr, etc      */
#include <stdio.h>      /* for fprintf()        */
#include <stdlib.h>     /* for exit()           */
#include "data_processor.h"

#define BUFF_SIZE 256
#define ALPHABET_SIZE 26
#define BARLENGTH 65
#define DELIMITER " "

long count_freq(char* file_name, long* char_freq, const char* dir_name)
{
    char file_path[BUFF_SIZE];
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_name, file_name);

    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        fprintf(stderr, "ERROR: Failed to open file");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "ERROR: Failed to close file.\n");
        exit(EXIT_FAILURE);
    }

    return char_count;
}

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

int send_data(char* buff, long arr[])
{
    int bytes, len;
    sprintf(buff, "%s", array_to_string(arr, ALPHABET_SIZE));
    len = strlen(buff) + 1;
    if ((bytes = write(STDOUT_FILENO, buff, len)) != len) {
        fprintf(stderr,
                "ERROR: Failed to write %d bytes, only sent %d bytes\n",
                len, bytes);
        return -1;
    }
    return bytes;
}

void receive_data(char* buff, long arr[])
{
    ssize_t read_result = read(STDIN_FILENO, buff, BUFF_SIZE);
    if(read_result < 0){
        fprintf(stderr, "ERROR: Failed to read pipe data.\n");
        exit(EXIT_FAILURE);
    } else if (read_result == 0) {
        fprintf(stderr, 
                "ERROR: Reached end of pipe without reading any data.\n");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(buff, DELIMITER);

    for(int i = 0; i < ALPHABET_SIZE; i++){
        if (!token) {
            fprintf(stderr, "ERROR: Received incomplete data.\n");
            exit(EXIT_FAILURE);
        }

        arr[i] += atol(token);

        token = strtok(NULL, DELIMITER);
    }
    
    if (token != NULL) {
        fprintf(stderr, "ERROR: Received too much data.\n");
        exit(EXIT_FAILURE);
    }
}