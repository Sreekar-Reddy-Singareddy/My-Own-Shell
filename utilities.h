#include <stdio.h>

#define EXEC_FILE_PATH "/home/011/s/sx/sxs190008/CS5348/Project_1/My-Own-Shell/executables.txt"

int horizonLine();

int horizonLine() {
  puts("======================================================");
  return 0;
}

void error_occured() {
//    char err_msg[30] = "An error has occured!!!\n";
//    write(STDERR_FILENO, err_msg, strlen(err_msg));
    puts("An error has occured\n");
}
