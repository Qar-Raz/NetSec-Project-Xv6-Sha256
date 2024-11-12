#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/riscv.h"

int main() {
    uint64 start_time, end_time;
    start_time = r_time();  // Get the initial time

      // Wait for 1 second

    end_time = r_time();  // Get the end time
    uint64 overall = end_time - start_time;

    printf("# of ticks in 1 second = %lu\n", overall);  // Correct format specifier
    return 0;
}
