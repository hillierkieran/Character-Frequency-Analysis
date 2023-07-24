
#include <stdio.h>      /* for fprintf()        */
#include <stdlib.h>     /* for exit()           */
#include <string.h>     /* for strcmp()         */

#define BUFF_SIZE 256
#define ALPHABET_SIZE 26
#define BARLENGTH 50
#define DELIMITER " "

char* array_to_string(long arr[], size_t length) {
    // calculate the length of the string
    int string_len = length * (sizeof(long) * 8 + 1);  // space for each digit and a space
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

int send_data(char* buff, long* char_freq)
{
    int bytes, len;
    sprintf(buff, "%s", array_to_string(char_freq, ALPHABET_SIZE));
    len = strlen(buff) + 1;
    if ((bytes = write(STDOUT_FILENO, buff, len)) != len) {
        fprintf(stderr,
                "ERROR: Failed to write %d bytes, only sent %d bytes\n",
                len, bytes);
        return -1;
    }
    return bytes;
}

void receive_data(int i, char* buff, long* received_freq)
{
    ssize_t read_result = read(STDIN_FILENO, buff, BUFF_SIZE);
    if(read_result < 0){
        fprintf(stderr, "ERROR: Process %d failed to read pipe data.\n", i);
        exit(EXIT_FAILURE);
    } else if (read_result == 0) {
        fprintf(stderr, "ERROR: Process %d reached end of pipe without reading any data.\n", i);
        exit(EXIT_FAILURE);
    }

    char* token = strtok(buff, DELIMITER);

    for(int i = 0; i < ALPHABET_SIZE; i++){
        if (!token) {
            fprintf(stderr, "ERROR: Process %d received incomplete data.\n", i);
            exit(EXIT_FAILURE);
        }

        received_freq[i] += atol(token);

        token = strtok(NULL, DELIMITER);
    }
    
    if (token != NULL) {
        fprintf(stderr, "ERROR: Process %d received too much data.\n", i);
        exit(EXIT_FAILURE);
    }
}
