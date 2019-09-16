#include <stdio.h>

#define EXEC_FILE_PATH "/cs5348-xv6/sxs190008/P1/executables.txt"

void error_occured();

void error_occured() {
//    char err_msg[30] = "An error has occured!!!\n";
//    write(STDERR_FILENO, err_msg, strlen(err_msg));
    puts("An error has occured\n");
}
