#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define NUM_EXCHANGES 10000

int main(void) {
    int pipe1[2], pipe2[2];
    char byte = 'A';
    int pid;
    int start_time, end_time;

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        printf(2, "Pipe creation failed\n");
        exit();
    }

    pid = fork();

    if (pid < 0) {
        printf(2, "Fork failed\n");
        exit();
    }

    start_time = uptime();

    if (pid == 0) {  // Child process
        close(pipe1[1]);
        close(pipe2[0]);

        for (int i = 0; i < NUM_EXCHANGES; i++) {
            if (read(pipe1[0], &byte, 1) != 1) {
                printf(2, "Child read failed\n");
                exit();
            }
            if (write(pipe2[1], &byte, 1) != 1) {
                printf(2, "Child write failed\n");
                exit();
            }
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit();
    } else {  // Parent process
        close(pipe1[0]);
        close(pipe2[1]);

        for (int i = 0; i < NUM_EXCHANGES; i++) {
            if (write(pipe1[1], &byte, 1) != 1) {
                printf(2, "Parent write failed\n");
                exit();
            }
            if (read(pipe2[0], &byte, 1) != 1) {
                printf(2, "Parent read failed\n");
                exit();
            }
        }

        close(pipe1[1]);
        close(pipe2[0]);
    }

    wait();
    end_time = uptime();

    int total_time = end_time - start_time;
    int exchanges_per_second = (NUM_EXCHANGES * 100) / total_time;  // Multiply by 100 for two decimal places

    printf(1, "Performance: %d.%d exchanges per second\n", exchanges_per_second / 100, exchanges_per_second % 100);

    exit();
}
