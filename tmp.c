void get_integers(char buff[], long *first, long *second)
{
    char *token = NULL;
    if ((first == NULL) || (second == NULL))
        return;
    if (read(STDIN_FILENO, buff, BUFFSIZE) > 0) {
        *first = atol((((token = strtok(buff, " ")) == NULL) ? "0" : token));
        *second = atol((((token = strtok(NULL, " ")) == NULL) ? "0" : token));
    } else {
        *first = 0;
        *second = 0;
    }
}
void safe_sum(long first, long *second, long *third)
{
    if ((second == NULL) || third == NULL)
        return;
    *third = first + *second;
    if (*third < 0) {
        fprintf(stderr,
                "Overflow detected: %ld + %ld != %ld\n",
                first, *second, *third);
        *third = 0;
        *second = 0;
    };
}
int send_data(char buff[], int cfreq[])
{
    int bytes, len;
    /* store file name into buffer */
    sprintf(buff, "%d", cfreq);
    len = strlen(buff) + 1;
    /* try write buffer into pipe for next process */
    if ((bytes = write(STDOUT_FILENO, buff, len)) != len) {
        fprintf(stderr,
                "Write of %d bytes failed, only sent %d bytes\n",
                len, bytes);
        return -1;
    } else {
        return bytes;
    }
}
void report_sum(int i, long first, long old_second, long second, long third)
{
    fprintf(stderr,
            "Process %d, recieved %ld %ld and sent %ld %ld\n",
            i, first, old_second, second, third);
}
long procFileCounts(char inFile[], long char_stats[]){

    char cur_fname[MAX_FILENAME] = TEXT_DIR;
    strcat(cur_fname, inFile);
    FILE* file = fopen(cur_fname, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
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
        char_count++;
    }
    return char_count;
}
