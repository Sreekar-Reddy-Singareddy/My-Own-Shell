#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "utilities.h"

int main (int count, char * args[]) {
    FILE * file;
    int i;
    file = fopen(EXEC_FILE_PATH, "w+");
    for (i=1; i<count; i++) {
        fputs(args[i], file);
        fputc('\n', file);
    }
    fclose(file);
}
