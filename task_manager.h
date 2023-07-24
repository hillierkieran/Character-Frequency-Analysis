/**
 * @file    task_manager.h
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * 
 * @brief   Header file for the task manager of a character counting program.
 * 
 * @details This file declares a function for dividing tasks evenly across all 
 *          processes in a ring architecture. Each process gets assigned tasks 
 *          based on the total number of tasks and the total number of processes.
 *          This ensures a balanced workload across all processes.
 */

int* divide_tasks(int i, int n_total, int n_procs, int* my_n_tasks);