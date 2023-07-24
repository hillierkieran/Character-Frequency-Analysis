/**
 * @file    data_processor.h
 * @brief   Header file for data_processor.c
 * 
 * @details This file declares the functions to be implemented in data_processor.c
 * 
 * @author  Kieran Hillier
 * @date    July 24, 2023
 */

long count_freq(char* file_name, long* char_freq, const char* dir_name);
char* array_to_string(long arr[], size_t length);
int send_data(char* buff, long arr[]);
int receive_data(char* buff, long arr[]);