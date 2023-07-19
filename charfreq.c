#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "ring.h"
#include "counter.h"

/*  Retrieve list of files in target directory (text files assumed) and 
    store this list of the file names in an array.                            */
/*  Build ring with user-specified number of nodes                            */
/*  Each node starts working on sub-set of files, 
    tallying frequency of each letter                                         */
/*  Once finished, nodes communicate:
        Main node sends array of frequencies around the ring
        Each ring node receives the array, adds their count, pass to next node
        Main node receives array then prints results to terminal              */
int main(int argc,  char *argv[ ]) {
    int   i;             /* number of this process (starting with 1)          */
    int   childpid;      /* indicates process should spawn another            */ 
    int   nprocs;        /* total number of processes in ring                 */

    // Check args contain: No. of processes in ring, and name of Dir with files  
    if(parse_args(argc,argv,&nprocs) < 0) exit(EXIT_FAILURE);

    // Build ring to user's specifications
    if(make_trivial_ring() < 0) {
        perror("Could not make trivial ring");
        exit(EXIT_FAILURE); 
    };
    for (i = 1; i < nprocs;  i++) {
        if(add_new_node(&childpid) < 0){
            perror("Could not add new node to ring");
            exit(EXIT_FAILURE); 
        };
        if (childpid) {
            break; 
        };
    };
    
    /* ring process code  */

    fprintf(stderr, "node %d of %d\n", i, nprocs);
    exit(EXIT_SUCCESS);
}