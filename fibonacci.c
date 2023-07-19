
#include <stdio.h>  /* for fprintf  */
#include <stdlib.h> /* for exit     */
#include <unistd.h> /* for fork     */
#include <string.h> /* for strtok   */
#define BUFFSIZE 50
int parse_args(int argc, char *argv[], int *np)
{
    if ((argc != 2) || ((*np = atoi(argv[1])) <= 0)) {
        fprintf(stderr, "Usage: %s nprocs\n", argv[0]);
        return (-1);
    };
    return (0);
}
int make_trivial_ring()
{
    int fd[2];
    if (pipe(fd) == -1)
        return (-1);
    if ((dup2(fd[0], STDIN_FILENO) == -1) ||
        (dup2(fd[1], STDOUT_FILENO) == -1))
        return (-2);
    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return (-3);
    return (0);
}
int add_new_node(int *pid)
{
    int fd[2];
    if (pipe(fd) == -1)
        return (-1);
    if ((*pid = fork()) == -1)
        return (-2);
    if (*pid > 0 && dup2(fd[1], STDOUT_FILENO) < 0)
        return (-3);
    if (*pid == 0 && dup2(fd[0], STDIN_FILENO) < 0)
        return (-4);
    if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        return (-5);
    return (0);
}
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
int send_numbers(char buff[], long num1, long num2)
{
    int bytes, len;
    sprintf(buff, "%ld %ld", num1, num2);
    len = strlen(buff) + 1;
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
            "\nProcess %d with PID %d and parent PID %d recieved %ld %ld and sent %ld %ld.\n",
            i, (int)getpid(), (int)getppid(), first, old_second, second, third);
}
int main(int argc, char *argv[])
{
    int i;               /* number of this process (starting with 1)   */
    int childpid;        /* indicates process should spawn another     */
    int nprocs;          /* total number of processes in ring          */
    char buff[BUFFSIZE]; /* string for integers                        */
    long first;          /* first integer in fibonacci computation     */
    long second;         /* second integer in fibonacci computation    */
    long third;          /* sum of first and second                    */
    if (parse_args(argc, argv, &nprocs) < 0)
        exit(EXIT_FAILURE);
    DIR 
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
        long old_second;
        if (i == 1)
        {
            /* mother of all processes */
            send_numbers(buff, 1, 1);
        }
        /* all processes     */
        get_integers(buff, &first, &second);
        old_second = second;
        safe_sum(first, &second, &third);
        if (i > 1)
        {
            send_numbers(buff, second, third);
            report_sum(i, first, old_second, second, third);
        }
        else
        {
            fprintf(stderr, "\n\nfibonacci(%d) = %ld\n\n", nprocs, second);
        }
        exit(EXIT_SUCCESS);
    }
} /* end of main program here */
