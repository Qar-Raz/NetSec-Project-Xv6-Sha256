#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "sha256.h"


volatile static int started = 0;

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
    
    
//----------------------------------------------SHA256 FOR KERNEL SPACE CODE
     char input[] = "kernel"; 
    uint8 hash[32];          // Buffer for the SHA-256 output
    SHA256_CTX ctx;          // SHA-256 context
    char hex_output[65];     // Buffer for the hexadecimal hash string
    int i;
    
int start_time = r_time();
    // SHA-256 computation steps
    sha256_init(&ctx);                            // Initialize SHA-256 context
    sha256_update(&ctx, (uint8*)input, strlen(input));  // Process the input string
    sha256_final(&ctx, hash);                     // Finalize the hash
int end_time = r_time();

int final_time = end_time - start_time;
    // Convert hash to hexadecimal string
    for (i = 0; i < 32; i++) {
        byte_to_hex(hash[i], &hex_output[i * 2]);
    }
    hex_output[64] = '\0';  // Null-terminate the string

    // Print the hash
    
    printf("\n");
    printf("\n");
    
    printf("Input for Kernel Space =  %s\n", input);
    printf("SHA-256 hash = %s\n", hex_output);
    printf("\nComputed in Ticks =  %d\n", final_time);

//------------------------------------------------SHA256 FOR KERNEL SPACE CODE

// could make this code into a function in proc.c for cleaner implementation --@Qamar
// we cannot give input to the kernel space implementation as that would have to be given from the userspace --@Qamar
    

    printf("\n");
    printf("\n");
    
// these print statements are added to provide instructions and for cleanness --@Qamar
   printf("-------Use sha256 sample.txt for user space implementation\n");
   printf("-------Use sha_syscall {string to be hashed} for system call implementation\n");
   
   printf("\n");
   printf("\n");


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
