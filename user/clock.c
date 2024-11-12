#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
    int time = uptime(); // Using the existing `uptime()` syscall.
    int seconds = time / 10;  // Assuming ticks are 100ms each.
    printf("Uptime: %d seconds\n", seconds);
    exit(0);
}
