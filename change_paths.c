#include<stdio.h>
#include<strings.h>
#include<stdlib.h>

void addPaths();

int main () {
    FILE * file;
    char * paths[15];
    size_t size = 100;
    
    // Get the current paths first
    file = fopen("/Users/sreekar/Desktop/S Drive/MS CS/OS_CS5348/Projects/My-Own-Shell/SAMPLE.txt", "r+");
    int i=0;
    paths[i] = (char *) malloc(50*sizeof(char));
    fgets(paths[i], 32, file);
    while (strcmp(paths[i], "") != 0) {
        printf("%d: %s",i+1,paths[i]);
        i++;
        paths[i] = (char *) malloc(50*sizeof(char));
        fgets(paths[i], 32, file);
    }
    fclose(file);
    
    // Ask the user for inputs
    char choice;
    printf("Would you like to add a new path (Y/N): ");
    choice = getchar();
    getchar();
    while (1) {
        if (choice == 'Y' || choice == 'y') {
            printf("Edit\n");
            addPaths();
            break;
        }
        else if (choice == 'N' || choice == 'n') {
            printf("Dont Edit\n");
            break;
        }
        else {
            printf("Would you like to add a new path (Y/N): ");
            choice = getchar();
            getchar();
        }
    }
}

void addPaths () {
    puts("In addPaths func");
    FILE * file;
    file = fopen("/Users/sreekar/Desktop/S Drive/MS CS/OS_CS5348/Projects/My-Own-Shell/SAMPLE.txt", "a+");
    
    size_t s = 100;
    char * input;
    input = (char *) malloc(100*sizeof(char));
    puts("Enter paths with : seperator");
    getline(&input, &s, stdin);
    puts(input);
    
    const char * DELIM = ":";
    char * paths[10];
    int i=0;
    paths[i] = strtok(input, DELIM);
    while (paths[i] != 0) {
        fputc('\n', file);
        fputs(paths[i], file);
        i++;
        paths[i] = strtok(0, DELIM);
    }
    fclose(file);
    
}
