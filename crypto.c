#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#ifdef __linux__
#include <time.h>
#include <sys/time.h>
#endif

#include "utils.h"
#include "crypto.h"

int encrypt(char **cypher_hex, char **message, int message_len, const char * key, int key_len)
{

    if (key_len != BLOCK_SIZE_BYTES)
    {
        print_invalid_key_length();
    }

    char * cypher = NULL; // encrypted plaintext
    cypher = (char *) calloc((message_len + 1), sizeof(char));

    int i;
    int cypher_len = message_len;
    int buffer_len;

    for (i = 0;i < message_len;i++)
    {
        *(cypher + i) = *(*message + i) ^ *(key + (i % BLOCK_SIZE_BYTES));
    }

    // fill with random chars
    if (is_not_multiple(i))
    {
        cypher_len += BLOCK_SIZE_BYTES - (i % BLOCK_SIZE_BYTES);
    }

    if (cypher_len > message_len)
    {
        cypher = (char *) realloc(cypher, (cypher_len + 1) * sizeof(char));
    }

    time_t t;
    srand(time(&t));

    if (cypher_len != message_len)
    {
        for (i = message_len; i < cypher_len; ++i)
        {
            *(cypher + i) = rand() % POSSIBLE_CHARS;
        }
    }

    *(cypher + cypher_len) = '\0';

    buffer_len = cypher_len * 2;

    // string has double so many characters in hex
    // ab -> 6162 -> buffer_len = cypher_len * 2
    *cypher_hex = (char *) calloc((buffer_len + 1), sizeof(char));

    string2hexString(cypher, *cypher_hex, cypher_len);

    free(cypher);

    return buffer_len;

}

int decrypt(char **message, char **cypher_hex, int cypher_hex_len, const char *key, int key_len)
{
    if (key_len != BLOCK_SIZE_BYTES)
    {
        print_invalid_key_length();
    }

    // must be multiple of 2 and BLOCK_SIZE
    if (cypher_hex_len % 2 != 0 || (cypher_hex_len/2) % BLOCK_SIZE_BYTES != 0)
    {
        printf("Kein gueltiger Chiffre-Text.\n");
        return -1;
    }

    int i;
    char *string_end = NULL;
    int cypher_len = cypher_hex_len / 2;
    int message_len = 0;

    char * cypher = NULL;
    cypher = (char *) calloc((cypher_len + 1), sizeof(char));
    *message = (char *) calloc((message_len + 1), sizeof(char));

    if (cypher == NULL || *message == NULL)
    {
        free(cypher);
        return -1;
    }


    hexString2string(*cypher_hex, cypher);

    for (i = 0;i < cypher_len;i++)
    {
        *(cypher + i)  = *(cypher + i) ^ *(key + (i % BLOCK_SIZE_BYTES));
    }

    // -------------------------------------------
    // several messages in one string
    // example: hallo welt & hallo vom mond
    // 2C366C6C6F20776528235F66696E5F012C366C6C6F20766F29776D6F6E645F662D395F23424B3E70

    int max_messages = 10;

    ptrdiff_t current_position = 0;
    ptrdiff_t c_len;
    ptrdiff_t next_message;

    while (1)
    {
        if (--max_messages <= 0)
        {
            printf("Es koennen maximal %d Nachrichten hintereinanden entschluesselt werden.", max_messages);
            break;
        }

        string_end = strstr( cypher + current_position, STRING_END);
        if (string_end == NULL) {
            if (current_position == 0)
            {
                return -1;
            }
            break;
        }

        ptrdiff_t bytes = string_end - (cypher + current_position);

        *message = (char *) realloc(*message, ((message_len + 1 + bytes) * sizeof(char)));
        if (*message == NULL)
        {
            free(cypher);
            return -1;
        }

        for (i = 0;i < bytes;i++)
        {
            *(*message + message_len + i) = *(cypher + current_position + i);
        }

        message_len += bytes;

        int random_bytes_len = (int) ((cypher_len - current_position) - (bytes + strlen(STRING_END)));

        if (random_bytes_len > (BLOCK_SIZE/8))
        {

            c_len = bytes + strlen(STRING_END);
            next_message = 0;

            if (is_not_multiple(c_len))
            {
                next_message += BLOCK_SIZE_BYTES - (c_len % BLOCK_SIZE_BYTES);
            }

            current_position += c_len + next_message;

        } else {
            break;
        }

    }

    *(*message + message_len) = '\0';

    free(cypher);

    return message_len;

}

int brute_force (char ** cypher_hex, int cypher_hex_len) {

    if (cypher_hex_len % 2 != 0)
    {
        printf("no valid cypher text");
        return -1;
    }

#ifdef __linux__

    // stop time
    struct timeval start, stop;

#endif

    double secs = 0;
    FILE *fp;
    int found = 0;

    char * key = NULL;
    int key_len = BLOCK_SIZE_BYTES;

    unsigned int possible = POSSIBLE_CHARS;

    char * cypher = NULL; // cypher text
    int cypher_len = cypher_hex_len / 2;

    char * message = NULL; // plaintext
    int message_len;

    char * print_able_key = NULL; // key in hex


    cypher = (char *) calloc((cypher_len + 1), sizeof(char));
    if (cypher == NULL) return -1;

    print_able_key = (char *) calloc((key_len * 2), sizeof(char));
    if (print_able_key == NULL) return -1;

    key = (char *) calloc((key_len + 1), sizeof(char));
    if (key == NULL) return -1;

    hexString2string(*cypher_hex, cypher);

    fp = fopen("found.txt", "w");
    if(fp == NULL)
    {
        printf("error opening the file found.txt\n");
        return -1;
    }

#ifdef __linux__
    // start timer
    gettimeofday(&start, NULL);
#endif

    int run = 1;
    int brute_force_bytes = RANDOM_BYTES - 1;
    int trials = 0;
    int k;

    while (run)
    {

        // increase the key by one
        //       1   [2]   3    4    5    6    7    8    (RANDOM_BYTES)
        // key = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
        // key = 0x00 0x01 0x00 0x00 0x00 0x00 0x00 0x00
        // ...
        // key = 0x00 0x7F 0x00 0x00 0x00 0x00 0x00 0x00 (0x7F = 127 = POSSIBLE_CHARS)
        // key = 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00
        // key = 0x01 0x01 0x00 0x00 0x00 0x00 0x00 0x00
        // ...
        // key = 0x01 0x7F 0x00 0x00 0x00 0x00 0x00 0x00
        // key = 0x02 0x00 0x00 0x00 0x00 0x00 0x00 0x00
        // key = 0x02 0x01 0x00 0x00 0x00 0x00 0x00 0x00
        // ...
        // key = 0x7F 0x7F 0x00 0x00 0x00 0x00 0x00 0x00

        for (k = brute_force_bytes; k >= 0; --k) {

            if (*(key + k) == (char) possible)
            {

                for (int l = 0; l < brute_force_bytes; ++l) {

                    if (*(key + l) < (char) possible) break;
                    if (l == brute_force_bytes - 1)
                    {
                        run = 0;
                        break;
                    }

                }
                if (run == 0) break;

                *(key + k) = (char) 0;

            } else {
                *(key + k) += (char) 1;

                break;

            }

        }

        // Shorten Brute Force, by knowing the plain text ([a-z ])
        // k is the position of the last changed letter
        if (!is_valid_char((*cypher + k) ^ *(key + k))) continue;

        trials++;

        message_len = decrypt(&message, &(*cypher_hex), cypher_hex_len, key, key_len);

        if(message_len == -1 || !is_valid_string(message, message_len)) continue;

        string2hexString(key, print_able_key, key_len);

        if (found == 0) { // Task in case only one key is found
            printf("1. Treffer: %s \t [key=0x%s]\n", message, print_able_key);
        }

        fprintf(fp, "%s \t [key=0x%s]\n", message, print_able_key);
        found++;

    }


#ifdef __linux__
    // stop time
    gettimeofday(&stop, NULL);
    secs = (double) (stop.tv_usec - start.tv_usec) / 1000000 + (double) (stop.tv_sec - start.tv_sec);
#endif

    if (found > 1)
    {
        printf("Weitere moegliche Treffer sind in der Datei found.txt.\n");
    }

    printf("Bei %d Versuchen wurde(n) %d moegliche(r) Schluessel in %fs gefunden.\n", trials, found, secs);

    fclose(fp);

    free(message);
    free(cypher);
    free(key);
//    free(print_able_key);

    return found;

}