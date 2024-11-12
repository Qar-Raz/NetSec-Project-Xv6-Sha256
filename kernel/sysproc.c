#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sha256.h"


uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.

extern struct spinlock tickslock;
extern uint ticks;

uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);

  return xticks;
}

uint64
sys_time(void) {
uint64 current_time = r_time();  // Call r_time to get the current timer value
    return current_time; 


}
uint64
sys_getmem(void) {
    int page_count;
    page_count = countpages(); // countpages() returns the number of free pages
				// it is defined in kernel/kalloc.c
				// it is also placed in kernel/defs.h so that it can be accessed 
                                //here or any place with kernel/defs.h header
    int megabytes;
    megabytes = (page_count * 4096) / (1024 * 1024); // Convert bytes to MB
    return megabytes;
}

uint64
sys_get_sha256(void) {
    char input[100];  // Buffer to store input from user program
    uint8 hash[32];
    SHA256_CTX ctx;
    char hex_output[65];
    int i;

    // Use argstr to retrieve the string argument
    if (argstr(0, input, sizeof(input)) < 0) {
        printf("Failed to retrieve input\n");
        return -1; // Error code if argument retrieval fails
    }

    // SHA-256 computation steps
    sha256_init(&ctx);
    sha256_update(&ctx, (uint8*)input, strlen(input));
    sha256_final(&ctx, hash);

    for (i = 0; i < 32; i++) {
        byte_to_hex(hash[i], &hex_output[i * 2]);
    }
    hex_output[64] = '\0';

    printf("SHA-256 hash: %s\n", hex_output);

    return 0;
}
