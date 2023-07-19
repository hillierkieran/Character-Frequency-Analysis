#include <dirent.h> /* for directories      */
#include <stdio.h>  /* for fprintf          */
#include <stdlib.h> /* for exit             */
#include <string.h> /* for strcmp           */
#include "ring.h"   /* for ring building    */
int parse_args(int argc, char *argv[], int *nprocs, char **dirName)
{
    if ((argc != 3) || ((*nprocs = atoi(argv[1])) <= 0)) {
        /* User input incorrect */
        fprintf(stderr, "Usage: %s nprocs dirname\n", argv[0]);
        return (-1);
    };
    /* Correct input */
    *dirName = argv[2];
    return (0);
}
size_t get_nfiles(char *dirName) 
{
    size_t i = 0;
    DIR *dirStream;
    struct dirent *entry;
    if ((dirStream = opendir(dirName)) == NULL ) {
        closedir(dirStream);
        return -1;
    }
    while ((entry = readdir(dirStream)) != NULL) {
        if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
            continue;
        i++;
    }
    closedir(dirStream); // close directory stream
    return i;
}
char** get_files(char *dirName, size_t n)
{
    int i = 0; 
    char **files = malloc(sizeof (char *) * n);
    if (!files)
        return NULL;
    DIR *dirStream;
    struct dirent *entry;
    if ((dirStream = opendir(dirName)) == NULL ) {
        fprintf(stderr, "can't open directory");
        return NULL;
    }
    while((entry = readdir(dirStream)) != NULL) {
        if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
            continue;
        else {
            files[i] = strndup(entry->d_name, strlen(entry->d_name));
            //files[i] = entry->d_name;
            if (!files[i]) {
                free(files);
                fprintf(stderr, "can't get entry");
                return NULL;
            }
            i++;
        }
    }
    for(i = 0; i < n; i++) 
        fprintf(stderr, " > %s\n", files[i]);
    closedir(dirStream); // close directory stream
    return files;
}
int main(int argc, char *argv[])
{
    int     i;          /* number of this process (starting with 1)     */
    int     childpid;   /* indicates process should spawn another       */
    int     nprocs;     /* total number of processes in ring            */
    char    *dirName;   /* sub-directory containing text files          */
    /* check user input is valid */
    if (parse_args(argc, argv, &nprocs, &dirName) < 0)
        exit(EXIT_FAILURE);
    /* Get number of files in given directory */
    size_t  nfiles = get_nfiles(dirName);
    if (!nfiles) {
        fprintf(stderr, "ERROR: Unable count number of files in given directory!\n");
        exit(1);
    }
    /* get names of files in given directory */
    char    **files = get_files(dirName, nfiles);
    if (!files) {
        fprintf(stderr, "ERROR: Unable to allocate files!\n");
        exit(1);
    }
    /* build ring */
    if (make_trivial_ring() < 0) {
        perror("Could not make trivial ring");
        exit(EXIT_FAILURE);
    };
    for (i = 1; i < nprocs; i++) {
        if (add_new_node(&childpid) < 0) {
            perror("Could not add new node to ring");
            exit(EXIT_FAILURE);
        };
        if (childpid)
            break;
    };
    {
        /* ring process code  */
        if (i == 1) {
            /* mother process */
        }
        /* all processes */
        //fprintf(stderr, "node %d of %d\n", i, nprocs);
        if (i > 1) {
            /* ring processes */
        } else {
            /* mother process */
        }
        exit(EXIT_SUCCESS);
    }
} /* end of main program here */