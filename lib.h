/* lib.h */

// Check arguments match the following: 
//   [1] number of processes to create, and 
//   [2] name of sub-directory containing text files
int parse_args(int argc,  char *argv[ ], int *np){
    if ( (argc != 3) || ((*np = atoi (argv[1])) <= 0) || argv[2]) {
        fprintf (stderr, "Usage: %s nprocs dirname\n", argv[0]);
        return(-1); 
    };
    
    return(0); 
}

// Make the first node
int make_trivial_ring(){   
    int   fd[2];
    if (pipe (fd) == -1) 
        return(-1); 
    if ((dup2(fd[0], STDIN_FILENO) == -1) ||
        (dup2(fd[1], STDOUT_FILENO) == -1)) 
        return(-2); 
    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))   
        return(-3); 
    return(0); 
}

// Add new node into existing ring
int add_new_node(int *pid){
    int fd[2];
    if (pipe(fd) == -1) 
        return(-1); 
    if ((*pid = fork()) == -1)
        return(-2); 
    if(*pid > 0 && dup2(fd[1], STDOUT_FILENO) < 0)
        return(-3); 
    if (*pid == 0 && dup2(fd[0], STDIN_FILENO) < 0)
        return(-4); 
    if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) 
        return(-5);
    return(0);
}

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