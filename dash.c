#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "commands.h"
#include "paths.h"
#define DELIM " "
#define PATH_DELIM ":"
#define DELIM_PARALLEL "&"

// hello there & whats up & how are you

// Function prototype declaration
int parse_command(char *);
char* check_command(char *);
int run_command(char *, char *, int);
int parse_input(char *);
int split_str(const char *, char *, char * []);
void remove_spaces(int, char * []);
int process_command(int, char * []);
int process_builtin_command(int, char * []);

// Collects input from the user.
int main (int count, char * args[]) {
    // Check if this is interactive or batch mode
    if (count == 2) {
        // Batch mode
        puts("Running in batch mode.");
        FILE * file;
        file = fopen(args[1], "r");
        char * command_line = (char *) malloc(100*sizeof(char));
        fgets(command_line, 100, file);
        while (strcmp(command_line, "") != 0) {
            printf("Line read: %s\n", command_line);
            command_line = strtok(command_line, "\n");
            parse_input(command_line);
            *command_line = '\0';
            fgets(command_line, 100, file);
        }
        exit(0);
        return 0;
    }
    else if (count == 1) {
        char * input; // Pointer to the input buffer given by the user
        size_t size = 100;
        input = (char *) malloc(size*sizeof(char));
        
        printf("dash (PID:: %d)> ",getpid());
        getline(&input, &size, stdin); // Read the input from the input source
        input = strtok(input, "\n"); // Just remove the unwanted newline character
        
        while (1) { // Run this forever
            parse_input(input);
            input[0] = '\0'; // Clear the string (virtually) after its used
            printf("dash (PID:: %d)> ",getpid()); // Continue to take another input
            getline(&input, &size, stdin); // Read the input from the input source
            input = strtok(input,"\n");
        }
    }
    else {
        printf("An error has occured\n");
        exit(0);
    }
}

// Takes the full line of input given by the user
// and parses it with '&'.
// Next it parses each command for arguments using ' '.
int parse_input (char * input) {
    // 1. Try splitting the input by '&'
    char * commands[20];
    int num_of_commands = split_str(DELIM_PARALLEL, input, commands); // This gives the number of parallel commands
    
    // 2. Filter each command by removing spaces
    remove_spaces(num_of_commands, commands); // This removes leading and trailing spaces from all commands
    
    // 3. Split each parallel command by ' '
    int i=0;
    while (i<num_of_commands) {
        char * components[10];
        int num_of_comps = split_str(DELIM, commands[i], components);
        // Check if this command is an exit built in command
        if (components[0]) {
            if (strcmp(components[0], "exit") == 0) { // Builtin Command - Exit the dash
                if (num_of_comps != 1) { // exit MUST have NO arguments
                    printf("Invalid number of args for %s\n", components[0]);
                    return -1;
                }
                puts("Adios Amigo!\n");
                exit(0);
            }
        }
        // Create a new process for each command and run it in there
        if (fork() == 0) {
            process_command(num_of_comps, components);
            exit(0);
        }
        else {
            wait(NULL);
        }
        i++;
    }
    
    return 0;
}

int process_builtin_command (int count, char * components[]) {
    printf("'%s' command runs in process with PID = %d\n", components[0], getpid());
    return -1;
}

// This processes a command by checking if it can be
// run or not. If it can be run, it gets the path
// from where the command can be run.
// Each command runs in its own child process.
int process_command (int count, char * components[]) {
    printf("'%s' command runs in process with PID = %d\n", components[0], getpid());
    if (strcmp(components[0], "cd") == 0) { // Builtin Command - change the current directory
        if (count != 2) { // cd MUST have only 1 argument
            printf("Invalid number of args for %s\n", components[0]);
            return -1;
        }
        chdir(components[1]);
        return 0;
    }
    else if (strcmp(components[0], "path") == 0) { // Builtin Command - change the path of the executables
        run_command("change_paths", components, 0);
        return 0;
    }
    
    // Before proceeding to run the command, check for the exec file
    char * exec_file_path;
    exec_file_path = (char *) malloc(30*sizeof(char));
    strcpy(exec_file_path, check_command(components[0]));
    printf("The command can be run from: %s\n", exec_file_path);
    if (strcmp(exec_file_path, "None") == 0) {
        printf("%s command not found\n",components[0]);
        return -1;
    }
    
    // Executable found. Run it.
    run_command(exec_file_path, components, 0);
    return 0;
}

// Take this command and check if it is available
// in any of the paths in SAMPLE.txt.
// If exists, return the full path. Else, return "None".
char* check_command (char * cmd) {
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
            fclose(file);
            return paths[i];
        }
        i++;
        paths[i] = (char *) malloc(50*sizeof(char));
        fgets(paths[i], 32, file);
    }
    fclose(file);
    return "None";
}

// Takes the path of the executable command
// along with the arguments. Runs the command.
int run_command (char * path, char * args, int outputTarget) {
    if (outputTarget == 1) { // It means the output must go into a file
        freopen("output.txt", "w+", stdout);
    }
    else {
        freopen("/dev/tty", "w+", stdout);
    }
    horizonLine();
    int res = execv(path, args);
    // Below code executes only if execv() fails
    freopen("/dev/tty", "w+", stdout);
    printf("Returned from Execv: %d\n",res);
    exit(res);
    return 0;
}

// Takes a string and splits it using @delim and puts each
// token in comps[]
int split_str(const char * delim, char * input, char * comps[]) {
    int i=0;
    comps[i] = strtok(input, delim);
    while (comps[i] != 0) {
        printf("**%s**\n", comps[i]);
        i++;
        comps[i] = strtok(0, delim);
    }
    return i;
}

// Removes leading and trailing spaces of all strings in comps[]
void remove_spaces(int count, char * comps[]) {
    int i=0;
    while (i<count) {
        int leading=0, trailing=0;
        while (leading == 0 || trailing == 0) {
            // When the string is empty
            if (*comps[i] == '\0') {
                trailing = 1;
                leading = 1;
            }
            // Removes leading spaces
            if (*comps[i] == ' ') {
                comps[i] = comps[i] + sizeof(char);
            }
            else {
                leading = 1;
            }
            
            // Removes trailing spaces
            int l = strlen(comps[i]);
            if (*(comps[i]+(l-1)) == ' ') {
                *(comps[i]+(l-1)) = '\0';
            }
            else {
                trailing = 1;
            }
        }
        printf("**%s**\n", comps[i]);
        i++;
    }
}
