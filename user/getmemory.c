#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
  char hash[65];        // Buffer to hold hash from kernel
  char filehash[65];    // Buffer to hold hash from file
  int fd;
  int n;

  // Prepare a0 to pass user buffer address to kernel
  asm volatile("mv a0, %0" : : "r"(hash));

  // Get kernel boot hash
  if (getmem() < 0) {
    printf("Failed to get kernel hash.\n");
    exit(1);
  }

  // Open the sample.txt file
  fd = open("sample.txt", O_RDONLY);
  if (fd < 0) {
    printf("Failed to open sample.txt\n");
    exit(1);
  }

  // Read hash from file into filehash
  n = read(fd, filehash, sizeof(filehash) - 1);
  if (n < 0) {
    printf("Failed to read from sample.txt\n");
    close(fd);
    exit(1);
  }

  filehash[n] = '\0'; // Null-terminate the file content
  close(fd);

  // Remove newline if present
  if (filehash[n - 1] == '\n') {
    filehash[n - 1] = '\0';
  }

  // Print what was read from the file
  printf("Read from sample.txt: %s\n", filehash);

  // Compare hashes
  if (strcmp(hash, filehash) == 0) {
    printf("Hash MATCHES with sample.txt!\n");
  } else {
    printf("Hash DOES NOT match sample.txt.\n");
    printf("From kernel:    %s\n", hash);
    printf("From file:      %s\n", filehash);
  }

  exit(0);
}

