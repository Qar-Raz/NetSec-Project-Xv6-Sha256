#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    uint64 time_us = uptime();
    printf("Uptime = %lu\n", time_us);
    return 0;
}
