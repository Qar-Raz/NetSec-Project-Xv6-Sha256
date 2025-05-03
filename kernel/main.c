#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "sha256.h" // Your hash function header

// <<< ADD THESE LINES >>>
// Declare the embedded data and its length (defined in kernel/embedded_sleeplock.c)
// These variables are created by the xxd command in the Makefile.
extern unsigned char kernel_subset_sources_tmp[];
extern unsigned int kernel_subset_sources_tmp_len;
// <<< END OF ADDED LINES >>>

volatile static int started = 0;

// Function prototype for byte_to_hex (if not already in defs.h)
// Ensure this function is defined somewhere accessible (e.g., kernel/printf.c)
void byte_to_hex(uint8 b, char *hex);

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk


//----------------------------------------------SHA256 FOR EMBEDDED KERNEL FILE CONTENT
    uint8 hash[32];          // Buffer for the SHA-256 output
    SHA256_CTX ctx;          // SHA-256 context
    char hex_output[65];     // Buffer for the hexadecimal hash string
    int i;

    // Use the embedded data declared above
    printf("Hashing embedded content of kernel/sleeplock.c (%d bytes)\n",  kernel_subset_sources_tmp_len);

    int start_time = r_time(); // Assuming r_time() is your function to get time ticks
    // SHA-256 computation steps
    sha256_init(&ctx);                            // Initialize SHA-256 context

    // Use the embedded array and its length directly from memory
   sha256_update(&ctx, (uint8*)kernel_subset_sources_tmp, kernel_subset_sources_tmp_len);


    sha256_final(&ctx, hash);                     // Finalize the hash
    int end_time = r_time(); // Assuming r_time() is your function to get time ticks

    int final_time = end_time - start_time;

    // Convert hash to hexadecimal string (assuming byte_to_hex exists)
    // Ensure byte_to_hex is defined (e.g., in kernel/printf.c or kernel/utils.c)
    // and declared (e.g., in kernel/defs.h or above main here)
    for (i = 0; i < 32; i++) {
         byte_to_hex(hash[i], &hex_output[i * 2]);
    }
    hex_output[64] = '\0';  // Null-terminate the string

    store_boot_hash(hex_output);
    // Print the hash
    printf("SHA-256 hash in kernel: %s\n", hex_output);
    printf("Computed in Ticks = %d\n", final_time);
//------------------------------------------------SHA256 FOR EMBEDDED KERNEL FILE CONTENT


    // comments related to previous implementation (can be kept or removed)
    // could make this code into a function in proc.c for cleaner implementation --@Qamar
    // we cannot give input to the kernel space implementation as that would have to be given from the userspace --@Qamar

    printf("\n");
    printf("\n");

    // these print statements are added to provide instructions and for cleanness --@Qamar
    printf("-------Use sha256 sample.txt for user space implementation\n");
    printf("-------Use sha_syscall {string to be hashed} for system call implementation\n");

    printf("\n");
    printf("\n");

    // comments related to userinit (can be kept or removed)
    // userinit calls the function in proc.c which then goes into the initcode.S and this calls exec syscall to execute the user process /init --@Qamar
    // hence userinit takes us into the userspace, so we have to call sha256 before userinit --@Qamar
    // every process starts in the userspace. For sha256 in kernel space we need the execution to not be a process --@Qamar
    userinit();      // first user process

    __sync_synchronize();
    started = 1;

  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();
}


/*
// --- Example definition for byte_to_hex ---
// You might need to place this definition in a suitable file (like kernel/printf.c or kernel/string.c)
// and declare it in kernel/defs.h if it's not already available.
// Make sure it doesn't conflict with existing functions.

void
byte_to_hex(uint8 b, char *hex) {
    static const char hex_chars[] = "0123456789abcdef";
    hex[0] = hex_chars[(b >> 4) & 0xF];
    hex[1] = hex_chars[b & 0xF];
    // hex[2] should be set to '\0' by the caller if printing single bytes,
    // but the loop in main handles the final null termination for the full string.
}
*/
