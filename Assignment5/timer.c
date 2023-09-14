// get input from user and count down to 0
// print the current time every second

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int time = atoi(argv[1]);
    while (time > 0) {
        printf("%d\n", time);
        sleep(1);
        time--;
    }
    return 0;
}