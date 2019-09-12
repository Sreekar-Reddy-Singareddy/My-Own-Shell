#include<stdio.h>
#include<strings.h>
#include<stdlib.h>

int main (int count, char * args[]) {
    FILE * file;
    int i;
    file = fopen("/Users/sreekar/Desktop/S Drive/MS CS/OS_CS5348/Projects/My-Own-Shell/SAMPLE.txt", "w+");
    for (i=1; i<count; i++) {
        fputs(args[i], file);
        if (i != count-1) fputc('\n', file);
    }
    fclose(file);
}
