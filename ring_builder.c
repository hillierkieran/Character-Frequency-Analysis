/**
 * @file    ring_builder.c
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * 
 * @brief   Functions to create and manage a process ring.
 *
 * @details These functions are used to build a ring structure of processes
 *          and add new nodes (processes) to the ring as needed. The ring 
 *          structure allows efficient parallel processing in the character
 *          count program.
 */

#include <unistd.h>     /* for fork     */
#include "ring_builder.h"

/**
 * @brief   Creates an initial trivial ring.
 *
 * @details This function creates a pipe and redirects the standard input 
 *          and output to the read and write ends of the pipe respectively. 
 *
 * @return  Returns 0 if the operation is successful, otherwise returns 
 *          a negative number.
 */
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

/**
 * @brief   Adds a new node (process) to the ring.
 *
 * @param   pid Pointer to a variable to store the new process id.
 *
 * @return  Returns 0 if the operation is successful, otherwise returns 
 *          a negative number.
 */
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