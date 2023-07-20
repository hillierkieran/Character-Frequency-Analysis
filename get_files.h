/* get_files.h */
#include <dirent.h> /* for directories      */
#include <stdio.h>  /* for fprintf          */

size_t get_n_files(char *dir_name) 
{
    size_t n_files = 0;
    DIR *dir_stream;
    struct dirent *entry;
    if ((dir_stream = opendir(dir_name)) == NULL ) {
        closedir(dir_stream);
        return -1;
    }
    while ((entry = readdir(dir_stream)) != NULL) {
        if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
            continue;
        n_files++;
    }
    /*  close directory stream */
    closedir(dir_stream);
    return n_files;
}

char** get_files(char *dir_name, size_t n_files)
{
    int i = 0; 
    char **files = malloc(sizeof (char *) * n_files);
    if (!files)
        return NULL;
    DIR *dir_stream;
    struct dirent *entry;
    if ((dir_stream = opendir(dir_name)) == NULL ) {
        fprintf(stderr, "can't open directory");
        return NULL;
    }
    while((entry = readdir(dir_stream)) != NULL) {
        if (!strcmp(entry->d_name,".") || !strcmp(entry->d_name,".."))
            continue;
        else {
            files[i] = strndup(entry->d_name, strlen(entry->d_name));
            if (!files[i]) {
                free(files);
                fprintf(stderr, "can't get entry");
                return NULL;
            }
            i++;
        }
    }
    /*  close directory stream */
    closedir(dir_stream);
    return files;
}