#ifndef CRYPTO_H
#define CRYPTO_H

#define STRING_END "_fin_"
#define BLOCK_SIZE 64
//#define BLOCK_SIZE 128
#define BLOCK_SIZE_BYTES (BLOCK_SIZE/8)

// Only the first 16 bits of the key are random, the rest can be skipped
#define RANDOM_BYTES 2

// currently the first bit in the key and in the message must be a 0
// -> 2^7 - 1 = 127
#define POSSIBLE_CHARS 127

int encrypt(char **cypher_hex, char **message, int message_len, const char * key, int key_len);
int decrypt(char **message, char **cypher_hex, int cypher_hex_len, const char * key, int key_len);
int brute_force (char ** cypher_hex, int cypher_hex_len);

#endif //CRYPTO_H