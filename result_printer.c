/* result_printer.c */
#include "result_printer.h"
#include <ctype.h>      /* */
#include <math.h>
#include <stdio.h>      /* for fprintf()        */
#include <stdlib.h>     /* for exit()           */
#include <string.h>     /* for strcmp()         */
#include <sys/wait.h>   /* for wait()           */
#include "arg_parser.h"
#include "make_ring.h"  /* for ring building    */
#include "get_files.h"  /* for getting files    */

#define BUFF_SIZE 256
#define ALPHABET_SIZE 26
#define BARLENGTH 65
#define DELIMITER " "

void print_bar_chart(char c, long count, long max_count) 
{
    int bar_length = (int) round( (double) count / max_count * BARLENGTH );
    fprintf(stderr, " %c: %7ld | ", c, count);
    for (int i = 0; i < bar_length; i++) {
        fprintf(stderr, "*");
    }
    fprintf(stderr, "\n");
}

void print_result(int n_procs, long char_freq[]) 
{
    fprintf(stderr, "\nProcessing complete on the ring with %d processes\n",
            n_procs);
    // Find the maximum count to scale the bar chart
    long max_count = 0;
    for (int j = 0; j < ALPHABET_SIZE; j++) {
        if (char_freq[j] > max_count) {
            max_count = char_freq[j];
        }
    }
    // Print the bar chart for each character's frequency
    for (int c = 0; c < ALPHABET_SIZE; c++) {
        print_bar_chart('a' + c, char_freq[c], max_count);
    }
    fprintf(stderr, "\n");
}
