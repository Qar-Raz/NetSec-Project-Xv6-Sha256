#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/riscv.h"

#define EXCHANGES 1000
//@ Qamar
int main() {
// create 2 pipeline with one read end and one write end
    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);

    uint64 start, end;  // Used for the time calculation

    start = time(); // Get the start time in ticks
	int temp = fork();
    if (temp != 0) {
        // Parent process
        close(p1[0]);  // Close read end of pipe 1
        close(p2[1]);  // Close write end of pipe 2
        char ping[1] = "P";

        for (int i = 0; i < EXCHANGES; i++) {
            write(p1[1], ping, 1);  // Send ping
		printf("Send Ping");
            read(p2[0], ping, 1);
		printf("Receive Ping");   // Wait for pong
        }
	    end = time(); // Get the end time in ticks

        wait(0);  // Wait for child to finish
        close(p1[1]);  // Close write end of pipe 1
        close(p2[0]);  // Close read end of pipe 2
    } else {
        // Child process
        close(p1[1]);  // Close write end of pipe 1
        close(p2[0]);  // Close read end of pipe 2
        char pong[1];

        for (int j = 0; j < EXCHANGES; j++) {
            read(p1[0], pong, 1);   // Wait for ping
            write(p2[1], pong, 1);  // Send pong
        }

        close(p1[0]);  // Close read end of pipe 1
        close(p2[1]);  // Close write end of pipe 2
        exit(0);
    }
// we close pipes to free up the file descriptor assigned to them and to
// prevent memory leaks


    int time_taken =( end - start)/100; // Calculate time
    int exchanges_persec = time_taken/ EXCHANGES;
    printf("\nTotal ticks: %d\n", time_taken);
    printf("Total Exchanges per second = %d\n",exchanges_persec);

    return 0;
}
