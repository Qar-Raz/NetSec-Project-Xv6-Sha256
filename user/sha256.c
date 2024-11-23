#include "kernel/types.h" // Include the kernel types header file for data types --@Qamar
#include "user/user.h" // Include the user header file --@Qamar
#include "kernel/fcntl.h" // Include the kernel file control header file --@Qamar
#include "kernel/syscall.h" // Include the kernel syscall header file to access syscall for file reading --@Qamar

// Macros to perform bitwise left and right rotations --@Qamar
#define ROTLEFT(a, b) ((a << b) | (a >> (32 - b))) // Rotate left --@Qamar
#define ROTRIGHT(a, b) ((a >> b) | (a << (32 - b))) // Rotate right --@Qamar

// Macros for SHA-256 specific bitwise operations --@Qamar
#define CH(x, y, z) ((x & y) ^ (~x & z)) // Choose function --@Qamar
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z)) // Majority function --@Qamar
#define EP0(x) (ROTRIGHT(x, 2) ^ ROTRIGHT(x, 13) ^ ROTRIGHT(x, 22)) // SHA-256 compression function 0 --@Qamar
#define EP1(x) (ROTRIGHT(x, 6) ^ ROTRIGHT(x, 11) ^ (ROTRIGHT(x, 25))) // SHA-256 compression function 1 --@Qamar
#define SIG0(x) (ROTRIGHT(x, 7) ^ ROTRIGHT(x, 18) ^ (x >> 3)) // SHA-256 message schedule function 0 --@Qamar
#define SIG1(x) (ROTRIGHT(x, 17) ^ ROTRIGHT(x, 19) ^ (x >> 10)) // SHA-256 message schedule function 1 --@Qamar

// SHA-256 constants --@Qamar
static const uint32 k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// Structure to hold SHA-256 context --@Qamar
typedef struct {
    uint8 data[64]; // Current data block --@Qamar
    uint32 datalen; // Length of the current data block --@Qamar
    uint64 bitlen; // Total length of the processed data in bits --@Qamar
    uint32 state[8]; // Hash state --@Qamar
} SHA256_CTX;

// Function to perform the SHA-256 transformation --@Qamar
void sha256_transform(SHA256_CTX *ctx, uint8 data[]) {
    uint32 a, b, c, d, e, f, g, h, i, j, t1, t2, m[64]; 

    // Prepare the message schedule from the input data --@Qamar
    for (i = 0, j = 0; i < 16; ++i, j += 4)
        m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
    for (; i < 64; ++i)
        m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

    // Initialize working variables with the current hash state --@Qamar
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    // Perform the main hash computation --@Qamar
    for (i = 0; i < 64; ++i) {
        t1 = h + EP1(e) + CH(e, f, g) + k[i] + m[i];
        t2 = EP0(a) + MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    // Update the hash state with the results of this chunk --@Qamar
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

// Function to initialize the SHA-256 context --@Qamar
void sha256_init(SHA256_CTX *ctx) {
    ctx->datalen = 0; // Initialize datalen to 0 --@Qamar
    ctx->bitlen = 0; // Initialize bitlen to 0 --@Qamar
    ctx->state[0] = 0x6a09e667; 
    ctx->state[1] = 0xbb67ae85; 
    ctx->state[2] = 0x3c6ef372; 
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f; 
    ctx->state[5] = 0x9b05688c; 
    ctx->state[6] = 0x1f83d9ab; 
    ctx->state[7] = 0x5be0cd19; 
}

// Function to update the SHA-256 context with new data --@Qamar
void sha256_update(SHA256_CTX *ctx, uint8 data[], uint len) {
    for (uint i = 0; i < len; ++i) {
        ctx->data[ctx->datalen] = data[i]; // Copy data to the context --@Qamar
        ctx->datalen++; // Increment datalen --@Qamar
        if (ctx->datalen == 64) { // If the data block is full --@Qamar
            sha256_transform(ctx, ctx->data); // Transform the data --@Qamar
            ctx->bitlen += 512; // Update bitlen --@Qamar
            ctx->datalen = 0; // Reset datalen --@Qamar
        }
    }
}

// Function to finalize the SHA-256 hash and produce the digest --@Qamar
void sha256_final(SHA256_CTX *ctx, uint8 hash[]) {
    uint32 i = ctx->datalen; // Get the current datalen --@Qamar

    // Pad the remaining data --@Qamar
    if (ctx->datalen < 56) {
        ctx->data[i++] = 0x80; // Append a '1' bit --@Qamar
        while (i < 56) // Append '0' bits --@Qamar
            ctx->data[i++] = 0x00;
    } else {
        ctx->data[i++] = 0x80; // Append a '1' bit --@Qamar
        while (i < 64) // Append '0' bits --@Qamar
            ctx->data[i++] = 0x00;
        sha256_transform(ctx, ctx->data); // Transform the data --@Qamar
        memset(ctx->data, 0, 56); // Clear the data block --@Qamar
    }

    // Append the total length of the data --@Qamar
    ctx->bitlen += ctx->datalen * 8; // Update bitlen --@Qamar
    ctx->data[63] = ctx->bitlen; // Append bitlen --@Qamar
    ctx->data[62] = ctx->bitlen >> 8; 
    ctx->data[61] = ctx->bitlen >> 16; 
    ctx->data[60] = ctx->bitlen >> 24; 
    ctx->data[59] = ctx->bitlen >> 32; 
    ctx->data[58] = ctx->bitlen >> 40; 
    ctx->data[57] = ctx->bitlen >> 48; 
    ctx->data[56] = ctx->bitlen >> 56; 
    sha256_transform(ctx, ctx->data); // Transform the data --@Qamar

    // Produce the final hash value --@Qamar
    for (i = 0; i < 4; ++i) {
        hash[i] = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 4] = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 8] = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
        hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
    }
}

// Function to convert a byte to a hexadecimal string --@Qamar
void byte_to_hex(uint8 byte, char* hex_str) {
    const char hex_chars[] = "0123456789abcdef"; // Hexadecimal characters --@Qamar
    hex_str[0] = hex_chars[(byte >> 4) & 0x0F]; // Convert high nibble to hex --@Qamar
    hex_str[1] = hex_chars[byte & 0x0F]; // Convert low nibble to hex --@Qamar
}



// the main function for the user space implementation --@Qamar

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("Error: cannot open file %s\n", argv[1]);
        exit(1);
    }

    char buffer[10000];
    int bytesRead;
    SHA256_CTX ctx;
    uint8 hash[32];
    char hex_output[65];
    int i = 0;


  while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
    // Remove the last character. The last character is a txt file is always a /n and we need to remove it as it could ruin the hash --@Qamar
    if (bytesRead > 0) {
        buffer[bytesRead - 1] = '\0';  
    }

    // Print the buffer, excluding the last character --@Qamar
    write(1, buffer, bytesRead - 1);  
}
	 
    if (bytesRead < 0) {
        printf("Error: reading file %s\n", argv[1]);
        close(fd);
        exit(1);
    }
    
    close(fd);
    
    
    int start_time = time();
    
    // these are the actual hashing functions -- @Qamar
    sha256_init(&ctx);
    sha256_update(&ctx,(uint8*) buffer, strlen(buffer));
    sha256_final(&ctx, hash);
    
    int end_time = time();
    
    int final_time = end_time - start_time;


// this for loop is for conveting the bytes to hex. Xv6 does not support hexadecimals so we have to output is as a string --@Qamar
    for (i = 0; i < 32; i++) {
        byte_to_hex(hash[i], &hex_output[i * 2]);
    }
    hex_output[64] = '\0'; // Null-terminate the string

    printf("\nSHA-256 hash =  %s\n", hex_output);
    printf("Computation in Ticks =  %d\n", final_time);


    return 0;
}
