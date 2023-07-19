#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define int MAX_FILENAME = 12;
#define char * TEXT_DIR = "text_files";

// Tally up the occurrences of the characters in a file
long procFileCounts(char inFile[], long char_stats[]){

    char cur_fname[MAX_FILENAME] = TEXT_DIR;
    strcat(cur_fname, inFile);
    FILE* file = fopen(cur_fname, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(1);
    }
    char c;
    long char_count = 0;
    /* for as long as we can get characters... */
    while((c=fgetc(file))) {

        /* break if end of file */
        if(c == EOF){ 
            break;
        }else if((tolower(c)-'a')>=0 && (tolower(c)-'a') < 26){
            char_stats[tolower(c)-'a'] += 1;
        }else{
            continue;
        }
        /* otherwise add one to the count of that particular character */
        char_count+=1;
    }
    return char_count;
}