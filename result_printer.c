/**
 * @file    result_printer.c
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * 
 * @brief   Provides functionality for printing results in the char_count program.
 *
 * @details This file contains functions to print a bar chart and the final result.
 *          These functions are used by the main char_count program to display 
 *          the character frequencies in the terminal.
 */

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "result_printer.h"

#define ALPHABET_SIZE 26
#define BARLENGTH 65

/**
 * @brief Prints a bar chart representing the character frequency.
 *
 * @param c - Character to be printed in the bar chart.
 * @param count - Number of times the character appeared.
 * @param max_count - The maximum count of any character to scale the bar chart.
 */
void print_bar_chart(char c, long count, long max_count) 
{
    int bar_length = (int) round( (double) count / max_count * BARLENGTH );
    fprintf(stderr, " %c: %7ld | ", c, count);
    for (int i = 0; i < bar_length; i++) {
        fprintf(stderr, "*");
    }
    fprintf(stderr, "\n");
}

/**
 * @brief Prints the final result of the character frequency count.
 *
 * @param n_procs - Number of processes used in the calculation.
 * @param char_freq - Array of character frequencies counted by the program.
 */
void print_result(int n_procs, long char_freq[]) 
{
    fprintf(stderr, "Processing complete on the ring with %d processes\n",
            n_procs);
    // Find the maximum count to scale the bar chart
    long max_count = 0;
    for (int j = 0; j < ALPHABET_SIZE; j++) {
        if (char_freq[j] > max_count) {
            max_count = char_freq[j];
        }
    }
    // Print the bar chart for each character's frequency
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        print_bar_chart('a' + i, char_freq[i], max_count);
    }
}