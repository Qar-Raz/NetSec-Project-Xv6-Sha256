// init: The initial user-level program

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
#include "kernel/fcntl.h"

char *argv[] = { "sh", 0 }; // Arguments for the shell

int
main(void)
{
  int pid, wpid;

  if(open("console", O_RDWR) < 0){
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  dup(0);  // stderr

  // --- Start: Execute getmemory before the shell ---
  printf("------------------------------------- Hash Validation Start\n");
  pid = fork(); // Fork a child process for getmemory
  if(pid < 0){
    printf("init: fork for getmemory failed\n");
    exit(1); // Exit init if fork fails (critical error)
  }
  if(pid == 0){
    // --- Child process ---
    char *argv_getmemory[] = { "getmemory", 0 }; // Arguments for getmemory
    exec("getmemory", argv_getmemory);
    // exec only returns on error
    printf("init: exec getmemory failed\n");
    exit(1); // Exit child if exec fails
  } else {
    // --- Parent process (init) ---
    // Wait specifically for the getmemory child process to finish
    // We use waitpid here for clarity, though wait(0) would likely work too
    // as it's the only child expected at this moment.
    do {
        wpid = wait((int *)0); // Wait for *any* child process
        if(wpid < 0){
           printf("init: wait for getmemory returned an error\n");
           exit(1); // Exit init if wait fails (critical error)
        }
    } while (wpid != pid); // Loop until the specific getmemory child exits

    printf("------------------------------------- Hash Validation End\n");
    // Now init continues to the shell loop...
  }
  // --- End: Execute getmemory before the shell ---


  // --- Original Shell Loop ---
  for(;;){
    printf("init: starting sh\n");
    pid = fork(); // Fork a child process for the shell
    if(pid < 0){
      printf("init: fork for sh failed\n");
      exit(1); // Exit init if fork fails (critical error)
    }
    if(pid == 0){
      // --- Child process (shell) ---
      exec("sh", argv);
      // exec only returns on error
      printf("init: exec sh failed\n");
      exit(1); // Exit child if exec fails
    }

    // --- Parent process (init) ---
    // Wait for the shell or orphaned processes
    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0); // Wait for any child
      if(wpid == pid){
        // the shell exited; restart it by breaking the inner loop
        break;
      } else if(wpid < 0){
        printf("init: wait returned an error\n");
        exit(1); // Exit init if wait fails (critical error)
      } else {
        // it was a parentless process; reap it and continue waiting.
        printf("init: reaping zombie pid %d\n", wpid); // Optional: good for debugging
      }
    }
    // If we broke from the inner loop, the outer loop restarts the shell
  }

  // Should never reach here
  exit(0); // Included for completeness
}
