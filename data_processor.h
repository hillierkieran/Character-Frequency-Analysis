/* data_processor.h */

long count_freq(char* file_name, long* char_freq, const char* dir_name);
char* array_to_string(long arr[], size_t length);
int send_data(char* buff, long arr[]);
void receive_data(char* buff, long arr[]);