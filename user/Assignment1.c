#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int p1[2], p2[2];
    pipe(p1); // p1[0] is read end, p1[1] is write end
    pipe(p2); // p2[0] is read end, p2[1] is write end

    if (fork() != 0) {
        // parent process
        close(p1[0]);  // close unused read end of p1
        close(p2[1]);  // close unused write end of p2

        char ping = 'P';
        int start_ticks = uptime();  // get start time in ticks

        for (int i = 0; i < 100000; i++) {
            write(p1[1], &ping, 1);  // send "ping" to child
            read(p2[0], &ping, 1);   // receive "pong" from child
        }

        int end_ticks = uptime();  // get end time in ticks
        wait();  // wait for child process to finish

        close(p1[1]);  // close write end of p1
        close(p2[0]);  // close read end of p2

        // Calculate round-trip time (RTT) and exchanges per second
        int total_ticks = end_ticks - start_ticks;
        int rtt_ticks = total_ticks / 100000;

        printf(1, "Average RTT: %d ticks\n", rtt_ticks);
        printf(1, "Exchanges per second: %d times\n", 100000 / total_ticks);

    } else {
        // child process
        close(p1[1]);  // close unused write end of p1
        close(p2[0]);  // close unused read end of p2

        char pong;
        for (int j = 0; j < 100000; j++) {
            read(p1[0], &pong, 1);  // receive "ping" from parent
            write(p2[1], &pong, 1); // send "pong" to parent
        }

        close(p1[0]);  // close read end of p1
        close(p2[1]);  // close write end of p2
        exit();
    }

    exit();
}

