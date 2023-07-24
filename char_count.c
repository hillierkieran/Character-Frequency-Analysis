/* char_count.c */
#include <ctype.h>      /* */
#include <math.h>
#include <stdio.h>      /* for fprintf()        */
#include <stdlib.h>     /* for exit()           */
#include <string.h>     /* for strcmp()         */
#include <sys/wait.h>   /* for wait()           */
//#include "comms.h"      /* for communication    */
#include "make_ring.h"  /* for ring building    */
#include "get_files.h"  /* for getting files    */

#define BUFF_SIZE 256
#define ALPHABET_SIZE 26
#define BARLENGTH 50
#define DELIMITER " "

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
        fprintf(stderr, "ERROR: Invalid number of processes. Should be greater than 0.\n");
        return -1;
    }

    /* Correct input */
    *dir_name = argv[2];
    return 0;
}

int* divide_tasks(int i, int n_total, int n_procs, int* my_n_tasks)
{
    // Calculate the number of tasks each process should handle
    int tasks_per_proc = n_total / n_procs;
    int remainder = n_total % n_procs;

    *my_n_tasks = tasks_per_proc + ((i - 1) < remainder ? 1 : 0);
    int first = (i - 1) * tasks_per_proc + ((i - 1) < remainder ? (i - 1) : remainder);

    // Allocate memory for the list of task IDs
    int* task_ids = (int*)malloc(*my_n_tasks * sizeof(int));
    if (!task_ids) {
        fprintf(stderr, "ERROR: Failed to allocate memory for task IDs.\n");
        exit(EXIT_FAILURE);
    }

    // Assign task IDs to the list
    for (int j = 0; j < *my_n_tasks; j++) {
        task_ids[j] = first + j;
    }

    return task_ids;
}

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
    } else {
        return bytes;
    }
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

void printBarChart(int i, char c, long count, long max_count) {
    int bar_length = (int) round( (double) count / max_count * BARLENGTH );
    fprintf(stderr, "  Process %d got char %c: %7ld | ", i, c, count);
    for (int i = 0; i < bar_length; i++) {
        fprintf(stderr, "*");
    }
    fprintf(stderr, "\n");
}

void print_result(int i, long char_freq[]) 
{
    // Find the maximum count to scale the bar chart
    long max_count = 0;
    for (int j = 0; j < ALPHABET_SIZE; j++) {
        if (char_freq[j] > max_count) {
            max_count = char_freq[j];
        }
    }

    // Print the bar chart for each character's frequency
    for (int c = 0; c < ALPHABET_SIZE; c++) {
        printBarChart(i, 'a' + c, char_freq[c], max_count);
    }
}

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
        fprintf(stderr, "ERROR: Failed to count the number of files in the given directory!\n");
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

    /* all processes */
    //do_work(buff, i, n_files, n_procs, dir_name, files, char_freq);
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


    receive_data(i, buff, char_freq);

    if (i > 1) {
        /*  ring processes */
        send_data(buff, char_freq);
    } else {
        /*  mother process */
        /*  report total frequencies to terminal */
        fprintf(stderr, "\nProcessing complete on the ring with %d processes\n\n", n_procs);
        print_result(i, char_freq);
        fprintf(stderr, "\n");
    }

    /* Wait for all processes to finish */
    if (child_pid) {
        for (int j = 0; j < n_procs - 1; j++) {
            wait(NULL);
        }
    }
    
    /*  free this process' task memory after use */
    free(files);
    exit(EXIT_SUCCESS);
}