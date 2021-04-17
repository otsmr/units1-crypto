#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "utils.h"
#include "crypto.h"


void print_invalid_key_length ()
{
    printf("Der Schlüssel hat eine ungueltig laenge (muss %d Zeichen lang sein).", BLOCK_SIZE / 8);
    exit(0);
}

/**
 * convert a char string to a hex string
 *
 * @docs
 * sprintf: https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm
 *
 * @param input: abc
 * @param output: 61 62 63 (without space)
 */
void string2hexString(char* input, char* output, int input_len)
{

    int loop;
    int i = 0;

    for (loop = 0; loop < input_len; ++loop) {
        sprintf((char*) (output+i),"%02X", input[loop]);
        i+=2;
    }
    i++;
    output[i] = '\0';

}

/**
 * convert a hex string to a char string
 *
 * @param input: 61 62 63 (without space)
 * @param output: abc
 */

int hex_to_int (char c) {
    int result = (c / 16 - 3) * 10 + (c % 16);
    if (result > 9) result--;
    return result;
}

void hexString2string(char* input, char* output)
{

    int i = 0;
    char buf = 0;

    while(input[i] != '\0')
    {

        if( i % 2 != 0){
            output[(i+1) / 2 - 1] = hex_to_int(buf) * 16 + hex_to_int(input[i]);
        } else {
            buf = input[i];
        }
        ++i;

    }

    output[i / 2] = '\0';

}

/**
 * clear stdin before getting new input
 */
void clean_stdin(void)
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

/**
 * checks if the number is a multiple of the block size
 *
 * @param int number
 * @return int
 */
int is_not_multiple (int number)
{
    return (BLOCK_SIZE_BYTES - number % BLOCK_SIZE_BYTES) != BLOCK_SIZE_BYTES;
}

/**
 * checks if the character is valid
 *
 * @param char c
 * @return int
 */
int is_valid_char (char c) {

    if ((c <= 122 && c >= 97) || c == 32) return 1;

    return 0;

}

/**
 * checks the characters in a string
 *
 * @param string
 * @param string_len
 * @return int
 */
int is_valid_string(char *string, int string_len)
{

    if (string == NULL)
    {
        return 0;
    }

    for (size_t i = 0; i < string_len; i++)
    {
        if (!is_valid_char(string[i])) {
            return 0;
        }

    }
    return 1;
}


/**
 * read a string from the console
 *
 * @param char ** buffer
 * @param int is_message -> only allow is_valid_char and add custom string ending
 *
 * @return length from buffer
 */
int get_message(char **buffer, int is_message)
{

    int c;
    int i = 0;

    int buffer_len = 1;


    *buffer = (char *) calloc((buffer_len+1), sizeof(char));

    if (*buffer == NULL)
    {
        free(*buffer);
        return -1;
    }

    scanf("%c", *buffer);

    if (is_message)
    {
        *buffer[0] = (char) tolower(*buffer[0]);
        if (!is_valid_char(*buffer[0]))
        {
            i--; // the first char will be overwritten by the next char
        }
    }

    while (1)
    {

        c = getchar();
        if (c == '\n' || c == EOF)
        {
            break;
        }

        if (is_message)
        {
            c = tolower(c);
            if (!is_valid_char((char) c)) continue;
        }

        ++i;

        // update the buffer size
        buffer_len += 1;
        // tries to increase the size of the memory block by 1 char
        *buffer = (char *) realloc(*buffer, (buffer_len+1) * sizeof(char));

        if (*buffer == NULL)
        {
            free(*buffer);
            return -1;
        }

        *(*buffer + i) = (char) c;

    }

    if (is_message)
    {

        // add the string ending defined in STRING_END
        *buffer = (char *) realloc(*buffer, (buffer_len + 1 + strlen(STRING_END)) * sizeof(char));
        if (*buffer == NULL)
        {
            free(*buffer);
            return -1;
        }
        strcpy(*buffer+(buffer_len), STRING_END);

        buffer_len += strlen(STRING_END);

    }

    *(*buffer + buffer_len) = '\0';

    printf("\r\n");

    return buffer_len;

}