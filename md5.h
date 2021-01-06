#include <stdint.h>

#define BLOCK_LEN 64  // In bytes
#define STATE_LEN 4  // In words

void md5_hash(const uint8_t message[], size_t len, uint32_t hash[STATE_LEN]);
void md5_compress(uint32_t state[4], const uint8_t block[64]);
