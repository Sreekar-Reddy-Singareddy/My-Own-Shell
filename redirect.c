#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main () {
    freopen("output.txt", "a+", stdout);
    puts("Whats up man!!");
    freopen("/dev/tty", "a+", stdout);
    puts("Hello there!!");
}
