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

int main () {
  char * input; // Pointer to the input buffer given by the user
  size_t size = 100;
  input = (char *) malloc(size*sizeof(char));
  printf("dash (PID:: %d)> ",getpid());
  getline(&input, &size, stdin); // Read the input from the input source
  input = strtok(input, "\n"); // Just remove the unwanted newline character
  while (strcmp("exit",input) != 0) { // As long as the input is NOT "exit", take inputs
    parse_command(input);
    input[0] = '\0'; // Clear the string (virtually) after its used
    printf("dash (PID:: %d)> ",getpid()); // Continue to take another input
    getline(&input, &size, stdin); // Read the input from the input source
    input = strtok(input,"\n");
  }
  // 1. Why am I giving &input in the getline?
  // 2. Why am I not giving *input in order to print the string?
  // 3. *input gives character and input gives string. Why so?
}

// This method takes the input string given
// in the shell and parses it into command and args.
int parse_command (char * cmd) {
  horizonLine();
  printf("In parse_command func.\n");
  int i=0;
  char * inputs[20];
  inputs[i] = strtok(cmd, DELIM); // inputs[0] will always be the command to run. Ex: ls, pwd, etc...
  while (inputs[i] != 0) {
    printf(":::: %s\n",inputs[i]);
    i++;
    inputs[i] = strtok(0, DELIM);
  }
  horizonLine();

  // Before proceeding to split the paths, check for the exec file 
  char * whichPath;
  whichPath = (char *) malloc(20*sizeof(char));
  strcpy(whichPath, check_command(inputs[0]));                                                                                 
  printf("The command can be run from: %s\n", whichPath);
  if (whichPath == NULL) return -1;

  i=0;
  //char * path;
  //size_t size = 20;
  //path = (char *) malloc(size*sizeof(char));
  //strcpy(path,"/bin/");
  //printf("Resulting path: %s\n",path);
  horizonLine();

  int pid = fork();
  if (pid == 0) { // Child process runs here
    printf("Inside the child. -> ");
    //strcat(path, inputs[0]);
    //printf("Look up path: %s\n", path);
    //horizonLine();
    int res = execv(whichPath, inputs);
    printf("Returned from Execv: %d\n",res);
    exit(res);
  }
  else {
    wait(NULL);
  }
}

// Take this command and check if it is available
// in any of the paths mentioned in PATH variable 
// of "paths.h" header file.
// If exists, return 0. Else, return -1.
char* check_command (char * cmd) {
  horizonLine();
  puts("In check_command func.");
  int i=0;
  char * path1, * path2;
  char * path;
  path = (char *) malloc(100*sizeof(char));
  path1 = (char *) malloc(20*sizeof(char));
  path2 = (char *) malloc(20*sizeof(char));
  strcpy(path, PATH);
  strcpy(path1, strtok(path, PATH_DELIM)); // Split the PATH string at ':'
  strcpy(path2, strtok(0, PATH_DELIM));
  printf("Command: %s\n", cmd);
  strcat(path1, cmd);
  strcat(path2, cmd);
  printf("Path 1: %s, Path 2: %s\n", path1, path2);
  horizonLine();
  if (access(path1, X_OK) == 0) {
    return path1;
  }
  else if (access(path2, X_OK) == 0) {
    return path2;
  }
  return NULL;
}
