#include <stdio.h>
#define LS "ls"
#define CAT "cat"

int horizonLine();

int horizonLine() {
  puts("======================================================");
}

void error_occured() {
//    char err_msg[30] = "An error has occured!!!\n";
//    write(STDERR_FILENO, err_msg, strlen(err_msg));
    puts("An error has occured\n");
}
