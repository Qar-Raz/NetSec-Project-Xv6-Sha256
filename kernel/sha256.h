// sha256.h
#ifndef SHA256_H
#define SHA256_H

#include "kernel/types.h"

typedef struct {
    uint8 data[64];
    uint32 datalen;
    uint64 bitlen;
    uint32 state[8];
} SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, uint8 data[], uint len);
void sha256_final(SHA256_CTX *ctx, uint8 hash[]);
void byte_to_hex(uint8 byte, char* hex_str);

int retrieve_boot_hash(char *output_buffer, int buffer_size);
void store_boot_hash(char *calculated_hash_hex);
#endif
