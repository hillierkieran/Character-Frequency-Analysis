University of New England (UNE) - Trimester 2, 2023
# COSC530 Parallel and Distributed Computing <br /> Assignment 1 - Character Frequency Analysis


The Problem
-----------

_Character frequency analysis_ is a technique used to examine the occurrence and distribution of characters within a given text. By analyzing the frequency at which individual characters appear, valuable insights can be gleaned about the text's language, structure, and even the author's writing style. Frequency analysis is also employed in cryptoanalysis to break weak ciphers, such as simple substitution encoding, by observing the similarities between the frequencies of the encoded characters and those observed un-encoded samples of text.

![](https://upload.wikimedia.org/wikipedia/commons/thumb/d/d5/English_letter_frequency_%28alphabetic%29.svg/600px-English_letter_frequency_%28alphabetic%29.svg.png) <br />
_Figure 1._ A typical distribution of letters in English language text. Weak ciphers do not sufficiently mask the distribution, and this might be exploited by a cryptanalyst to read the message. Source: [Wikipedia - Frequency analysis](https://en.wikipedia.org/wiki/Frequency_analysis#/media/File:English_letter_frequency_(alphabetic).svg)

In this assignment you will construct a multi-process C program that will perform character frequency analysis on a set of text files in specified directory.

Your Program
------------

You will implement a multi-process program that uses the ring communication structure based on the `pipe( ... )` system call (introduced in [lecture 04](http://turing.une.edu.au/~cosc330/lectures/display_notes.php?lecture=04#1)) to accumulate a count of the occupance of each character (a-z) in a set of text files within a sub-directory. For simplicity, you can simply convert uppercase letters to lowercase for the purposes of counting occurrences. Your program should accept the **number of process** to create along with the name of the **sub-directory** containing the text files as command-line arguments.

The algorithm for you program is fairly simple:

*   The main process should retrieve a list of the files in the target directory (you can assume all files in this directory are text files) and store this list of the file names in an array.
*   The main process should then create each process and add it to the ring communication structure.
*   Each process (including the main process) should then begin working through a sub-set of the files, tallying up the frequency of each character.
*   Once each process has finished processing its allotted files, it will need to communicate its results:
    *   The main process will simply need to send its array of frequencies to the next process in the ring and wait to receive the results from last process in the ring.
    *   All other processes will need to receive the array of frequencies from the _previous_ process in the ring, add their counts to the totals for each character and pass this result to the _next_ process in the ring.
    *   The main process can then report the frequencies to the terminal.

We have introduced a library of helper functions to create an initial ring of processes (with 1 process):

    int make_trivial_ring(){   
      int   fd[2];
      if (pipe (fd) == -1) 
        return(-1); 
      if ((dup2(fd[0], STDIN_FILENO) == -1) ||
          (dup2(fd[1], STDOUT_FILENO) == -1)) 
        return(-2); 
      if ((close(fd[0]) == -1) || (close(fd[1]) == -1))   
        return(-3); 
      return(0); }
    
    

Add new processes to the ring and connect them up to the appropriate pipes:

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
    

A simple skeleton program that uses the ring of processes:

    int main(int argc,  char *argv[ ]){
       int   i;             /* number of this process (starting with 1)   */
       int   childpid;      /* indicates process should spawn another     */ 
       int   nprocs;        /* total number of processes in ring          */ 
       if(parse_args(argc,argv,&nprocs) < 0) exit(EXIT_FAILURE);
       if(make_trivial_ring() < 0){
         perror("Could not make trivial ring");
         exit(EXIT_FAILURE); };
       for (i = 1; i < nprocs;  i++) {
         if(add_new_node(&childpid) < 0){
           perror("Could not add new node to ring");
           exit(EXIT_FAILURE); };
         if (childpid) break; };
       /* ring process code  */
    
       fprintf(stderr, "node %d of %d\n", i, nprocs);
       exit(EXIT_SUCCESS);
    } 
    

A very simple helper function to tally up the occurances of the characters in a file:

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
    

You should also review the [fibonacci sequence example](http://turing.une.edu.au/~cosc330/lectures/display_notes.php?lecture=05#1) where we put our ring of processes to work.

The output of your program should look something like that presented below. I have run my program on a directory that contains a random assortment of of free ebooks. These are included in the [assignment directory](https://turing.une.edu.au/~cosc330/assignments/assignment_01/text_files/). The distribution looks the same as that in figure 1.

    [mwelch8@turing character_stats] $ ./char_count 4 text_files
    
    Processing complete on ring with 4 processes
    
            Process  1 got char a: 692391   | ***************************************                     
            Process  1 got char b: 134807   | ********                                                    
            Process  1 got char c: 207461   | ************                                                
            Process  1 got char d: 373069   | *********************                                       
            Process  1 got char e: 1075941  | ************************************************************
            Process  1 got char f: 188771   | ***********                                                 
            Process  1 got char g: 172157   | **********                                                  
            Process  1 got char h: 567501   | ********************************                            
            Process  1 got char i: 594687   | **********************************                          
            Process  1 got char j: 10493    | *                                                           
            Process  1 got char k: 76828    | *****                                                       
            Process  1 got char l: 370823   | *********************                                       
            Process  1 got char m: 236036   | **************                                              
            Process  1 got char n: 597631   | **********************************                          
            Process  1 got char o: 692079   | ***************************************                     
            Process  1 got char p: 145851   | *********                                                   
            Process  1 got char q: 9084     | *                                                           
            Process  1 got char r: 531129   | ******************************                              
            Process  1 got char s: 573744   | ********************************                            
            Process  1 got char t: 785160   | ********************************************                
            Process  1 got char u: 266169   | ***************                                             
            Process  1 got char v: 87887    | *****                                                       
            Process  1 got char w: 208194   | ************                                                
            Process  1 got char x: 12446    | *                                                           
            Process  1 got char y: 190450   | ***********                                                 
            Process  1 got char z: 5401     | * 
    
    [mwelch8@turing character_stats] $
    
    

The output includes a simple text-based histogram visualization to depict the relative frequencies of the characters. This is created by manipulating an array of '\*' characters that is printed to the screen - to calculate the number of '\*' characters in this array, use the expression `(char_count/max_count) * BARLENGTH`, where `char_count` is the count for a specific character, `max_count` is the count for the most frequent character and `BARLENGTH` is the maximum size of the chart.

Submission
----------

*   Your assignment will need to be submitted through the `submit` program on turing.
*   Please review the [instructions for the submit program](http://turing.une.edu.au/~cosc330/assignments/submit_instr.html) if you are not familiar with its use.
*   Make sure that you record a script of your program compiling and working correctly.
*   Confirm that the file sizes listed in the submission receipt are not 0Kb!

Tentative Marking Guide
-----------------------

**Solution Correctness - 70%**

*   Does your solution compile and run?
*   Does your solution produce the correct/expected output?
*   Does your solution use a ring of processes?
*   Does your solution achieve parallelism by conducting the processing concurrently?
*   Does you solution communicate its results around the ring?
*   Does solution divide the processing amongst the processes in a sensible way?
*   Does your program provide output with the character counts and a simple visualization?

**Quality of Solution - 15%**

**_BEWARE! - 5% Will be deducted for each offense (minimum of 0 for this section)_**

*   Is your code broken down into functions (e.g. not more than about 60 lines - excluding braces, comments and whitespace)
*   Have you generated general-purpose/reusable functions?
*   Have you grouped related functions into separate libraries?
*   Have you included a complete makefile with `clean` and `run` targets?
*   Does your makefile use -Wall -pedantic ?
*   Does the code compile without errors/warnings (e.g. with -Wall -pedantic)?
*   Is there error checking on all system calls, user inputs or source file content?
*   Does your solution take appropriate action if an error occurs (e.g. make every effort to save the situation)?
*   Have you avoided the use of hard-coded literals? (e.g. use #define macros)

**Documentation - 10%**

**_BEWARE! - 5% Will be deducted for each offense (minimum of 0 for this section)_**

*   Does your header block contain the author's name, the purpose of the program and a description of how to compile and run the solution.
*   Are identifiers named in a meaningful way?
*   Are any obscure constructs fully explained?
*   Does each function have a header block that explains the purpose, its arguments and return value?
*   Have you recorded a submission script (in the `submit` program) showing your assignment compiling and running?

**Source Formatting - 5%**

**_BEWARE! - 5% Will be deducted for each offense (minimum of 0 for this section)_**

*   Is your indentation consistent? Make sure that you use the `indent` utility on all source files.
*   Have blank lines been used so that the code is easy to read?
*   Are any lines longer than 80 characters? (`indent` can sort this out)
*   Are capitalization and naming conventions consistent?
