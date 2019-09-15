#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main () {
    char * input = (char *) malloc(100*sizeof(char));
    size_t s = 100;
    getline(&input, &s, stdin);
    
    char * comps[20];
    int i=0;
    comps[i] = strtok(input, "&");
    while (comps[i] != 0) {
        puts(comps[i]);
        i++;
        comps[i] = strtok(0, "&");
    }
}
