#ifndef UTILS_H
#define UTILS_H

void string2hexString(char* input, char* output, int input_len);
void hexString2string(char* input, char* output);
int get_message(char **buffer, int is_message);
int is_valid_char(char c);
int is_valid_string(char *string, int string_len);
void clean_stdin(void);
void print_invalid_key_length(void);
int is_not_multiple (int number);

#endif //UTILS_H
