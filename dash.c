#include <stdio.h>
#include <string.h>
#include "commands.h"
#define DELIM " "

int parse_command (char *);

int main () {
  printf("Command Code for CAT: %s\n", CAT);
  char * input; // Pointer to the input buffer given by the user
  size_t size = 100;
  input = (char *) malloc(size*sizeof(char));
  printf("dash> ");
  getline(&input, &size, stdin); // Read the input from the input source
  while (*input != 'q') { // As long as the input is NOT 'q', take inputs
    parse_command(input);
    printf("dash> "); // Continue to take another input
    getline(&input, &size, stdin); // Read the input from the input source
  }
  // 1. Why am I giving &input in the getline?
  // 2. Why am I not giving *input in order to print the string?
  // 3. *input gives character and input gives string. Why so?
}

// This method takes the input string given
// in the shell and parses it into command and args.
int parse_command (char * cmd) {
  int i=0;
  char * inputs[20];
  inputs[i] = strtok(cmd, DELIM);
  while (inputs[i] != 0) {
    printf(":::: %s\n",inputs[i]);
    i++;
    inputs[i] = strtok(0, DELIM);
  }
  i=0;
  int pid = fork();
  printf("PID::: %d\n", pid);
  if (pid == 0) { // Child process runs here
    printf("Inside child...");
    char * command = "/bin/";
    strcat(command , inputs[0]);
    printf("Final path of the command: %s\n", command);
    int res = execv(command, inputs);
    printf("Returned from Execv: %d\n",res);
  }
  else {
    wait(NULL);
  }
}
