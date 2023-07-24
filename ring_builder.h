/**
 * @file    ring_builder.h
 * @author  Kieran Hillier
 * @date    July 24, 2023
 * @version 1.0
 * 
 * @brief   Header file for ring building functions.
 *
 * @details This file declares functions that are implemented in ring_builder.c.
 *          These functions are used to build a ring structure of processes
 *          and add new nodes (processes) to the ring as needed.
 */

int make_trivial_ring();
int add_new_node(int *pid);