#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "crypto.h"

int main (void)  {

    char * message = NULL;
    int message_len;

    char * cypher = NULL;
    int cypher_len;

    int run = 1;
    char start = '\0';

    char * key = NULL;
    int key_len;



    // --- the key could then be read in here ---
//    while (1)
//    {
//        printf("Passwort eingeben: ");
//        key_len = get_message(&key, 0);
//
//        if (key_len != BLOCK_SIZE_BYTES)
//        {
//            print_invalid_key_length();
//        }
//        else break;
//    }
    // ---


    // --- hardcoded secret ---
    key_len = BLOCK_SIZE_BYTES;
    key = (char *) calloc((key_len + 1), sizeof(char));

    strcpy(key, "DW");
    key[key_len] = '\0';
    // --- hardcoded secret ---

    // abcdefghijklmnopqrstuvwxyz
    // 128 Bit: 2535636465666768696A6B6C6D6E6F703525737475767778797A5F66696E5F49
    // 64 Bit: 25356364656667682D3D6B6C6D6E6F7035257374757677783D2D5F66696E5F1B



    while (run) {

        printf("\nencrypt (e), decrypt (d), brute-force (b), quit (q): ");
        scanf("%c", &start);
        clean_stdin();

        switch (start) {

            case 'd':

                // later then leseNachricht()

                printf("verschluesselte Nachricht eingeben: ");

                cypher_len = get_message(&cypher, 0);
                if (cypher_len < 0) {
                    printf("memory error");
                    exit(1);
                }

                cypher_len = decrypt(&message, &cypher, cypher_len, key, key_len);
                if (cypher_len < 0)
                {
                    printf("Konnte nicht entschluesselt werden.");
                    break;
                }

                printf("Klartext: %s\r\n", message);

                break;

            case 'e':

                // later then schreibeNachricht()

                printf("Nachricht eingeben: ");

                message_len = get_message(&message, 1);
                if (message_len < 0) {
                    printf("memory error");
                    exit(1);
                }

                cypher_len = encrypt(&cypher, &message, message_len, key, key_len);
                if (cypher_len < 0)
                {
                    printf("memory error");
                    exit(1);
                }

                printf("verschluesselte Nachricht: %s\r\n", cypher);

                break;

            case 'b':

                printf("verschluesselte Nachricht eingeben: ");

                cypher_len = get_message(&cypher, 0);
                if (cypher_len < 0) {
                    printf("memory error");
                    exit(1);
                }

                brute_force(&cypher, cypher_len);

                break;

            case 'q':

                printf("Programm wird beendet.");
                run = 0;

                break;

            default:
                printf("Option nicht bekannt. ");
                break;

        }

    }

    free(message);
    free(cypher);
    free(key);

    return 0;

}
