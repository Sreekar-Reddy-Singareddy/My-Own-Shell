#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "commands.h"
#include "paths.h"
#define DELIM " "
#define PATH_DELIM ":"
int parse_command (char *);
char* check_command (char *);
int run_command(char * path, char * args[]);

int main () {
    char * input; // Pointer to the input buffer given by the user
    size_t size = 100;
    input = (char *) malloc(size*sizeof(char));
    
    printf("dash (PID:: %d)> ",getpid());
    getline(&input, &size, stdin); // Read the input from the input source
    input = strtok(input, "\n"); // Just remove the unwanted newline character
    
    while (1) { // Run this forever
        parse_command(input);
        input[0] = '\0'; // Clear the string (virtually) after its used
        printf("dash (PID:: %d)> ",getpid()); // Continue to take another input
        getline(&input, &size, stdin); // Read the input from the input source
        input = strtok(input,"\n");
    }
}

// This method takes the input string given
// in the shell and parses it into command and args.
int parse_command (char * inputCommand) {
    horizonLine();
    printf("In parse_command func.\n");
    int i=0;
    char * inputs[20];
    
    inputs[i] = strtok(inputCommand, DELIM); // inputs[0] will always be the command to run. Ex: ls, pwd, etc...
    while (inputs[i] != 0) {
        printf(":::: %s\n",inputs[i]);
        i++;
        inputs[i] = strtok(0, DELIM);
    }
    if (strcmp(inputs[0], "exit") == 0) { // Builtin Command - Exit the dash
        if (i != 1) { // exit MUST have NO arguments
            printf("Invalid number of args for %s\n", inputs[0]);
            return -1;
        }
        puts("Adios Amigo!\n");
        exit(0);
    }
    else if (strcmp(inputs[0], "cd") == 0) { // Builtin Command - change the current directory
        if (i != 2) { // cd MUST have only 1 argument
            printf("Invalid number of args for %s\n", inputs[0]);
            return -1;
        }
        chdir(inputs[1]);
        return 0;
    }
    else if (strcmp(inputs[0], "path") == 0) { // Builtin Command - change the path of the executables
        run_command("change_paths", inputs);
        return 0;
    }
    horizonLine();
    
    // Before proceeding to split the paths, check for the exec file
    char * whichPath;
    whichPath = (char *) malloc(30*sizeof(char));
    strcpy(whichPath, check_command(inputs[0]));
    printf("The command can be run from: %s\n", whichPath);
    if (strcmp(whichPath, "None") == 0) {
        printf("%s command not found\n",inputs[0]);
        horizonLine();
        return -1;
    }
    
    // Executable is found. So run it in a different process
    i=0;
    horizonLine();
    run_command(whichPath, inputs);
    horizonLine();
    return 0;
}

// Take this command and check if it is available
// in any of the paths mentioned in LIST of paths
// given by the user in SAMPLE.txt
// If exists, return 0. Else, return -1.
char* check_command (char * cmd) {
    horizonLine();
    puts("In check_command func.");
    int i=0;
    char * paths[20];
    FILE * file;
    
    // Read all paths from the list
    file = fopen("/Users/sreekar/Desktop/S Drive/MS CS/OS_CS5348/Projects/My-Own-Shell/SAMPLE.txt", "r+");
    paths[i] = (char *) malloc(50*sizeof(char));
    fgets(paths[i], 32, file);
    while (strcmp(paths[i], "") != 0) {
        // For every path, append the command
        paths[i] = strtok(paths[i], "\n");
        strcat(paths[i], cmd);
        printf("%d: %s\n",i+1,paths[i]);
        // For every path, check if the file exists
        if (access(paths[i], X_OK) == 0) {
            puts("File is executable!");
            return paths[i];
        }
        i++;
        paths[i] = (char *) malloc(50*sizeof(char));
        fgets(paths[i], 32, file);
    }
    fclose(file);
    horizonLine();
    return "";
}

int run_command (char * path, char * args) {
    int pid = fork();
    if (pid == 0) { // Child process runs here
        printf("Inside the child. -> ");
        //strcat(path, inputs[0]);
        //printf("Look up path: %s\n", path);
        //horizonLine();
        int res = execv(path, args);
        printf("Returned from Execv: %d\n",res);
        exit(res);
    }
    else {
        wait(NULL);
    }
}
