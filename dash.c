// Note: Instructions for a successful run
// 1. 'utilities.h' header file
// 2. 'path.c' source code
// 3. In order to work well, recompile 'dash.c' & 'path.c'
// 4. All the paths are stored in 'executables.txt' file in the current directory
// 5. All the outputs (*.txt) will also be created and saved in the current directory


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "utilities.h"
#include <sys/types.h>
#include <sys/wait.h>

#define DELIM " "
#define DELIM_PARALLEL "&"
#define PATH_BUILTIN_CMD "path"
#define CD_BUILTIN_CMD "cd"
#define EXIT_BUILTIN_CMD "exit"
#define EXECUTABLE_NOT_FOUND "None"
#define PATH_CMD_PATH "/cs5348-xv6/sxs190008/P1/path"

// Function prototype declaration
int parse_command(char *);
char* check_command(char *);
void run_command(char *, char * [], int);
int parse_input(char *);
int split_str(const char *, char *, char * []);
void remove_spaces(int, char * []);
int process_command(int, char * []);
int process_builtin_command(int, char * []);
int is_builtin_command(char *);
int has_multiple_files(char *);

// Path of the output file
char * OUTPUT_PATH;
int SHOULD_APPEND = 0;

// Collects input from the user.
int main (int count, char * args[]) {
    // Check if this is interactive or batch mode
    if (count == 2) {
        // Batch mode
        FILE * file;
        file = fopen(args[1], "r");
        if (errno != 0) {
            error_occured();
            exit(1);
        }
        char * command_line = (char *) malloc(100*sizeof(char));
        fgets(command_line, 100, file);
        while (strcmp(command_line, "") != 0) {
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
        
        while (1) { // Run this forever
            if (strcmp(input,"\n") != 0) {
                input = strtok(input,"\n");
                parse_input(input);
            }
            input[0] = '\0'; // Clear the string (virtually) after its used
            printf("dash (PID:: %d)> ",getpid()); // Continue to take another input
            getline(&input, &size, stdin); // Read the input from the input source
        }
    }
    else {
        error_occured();
        exit(1);
    }
}

// Takes the full line of input given by the user
// and parses it with '&'.
// Next it parses each command for arguments using ' '.
int parse_input (char * input) {
    // 0. Split the input by '>'. The output MUST have exactly 2 args
    int len = strlen(input);
    int k=0;
    int red_symbs = 0;
    while (k<len) {
        if (*(input+k) == '>') red_symbs++;
        k++;
    }
    if (red_symbs > 1) {
        error_occured();
        return -1;
    }
    char * redirects[2];
    int num_of_redirects = split_str(">", input, redirects); // Split them by '>'
    remove_spaces(num_of_redirects, redirects); // Remove leading and trailing spaces
    redirects[0] = strtok(redirects[0], "\n"); // Cutoff newline character
    if (redirects[1] != NULL) {
        int multiple_files = has_multiple_files(redirects[1]); // Checks if there are multiple files given
        if (multiple_files == 1) {
            error_occured();
            return -1;
        }
    }
    // Copy the path of the output file and save in global variable
    // for future access.
    OUTPUT_PATH = redirects[1]; // This could be NULL.
    if (num_of_redirects > 2) {
        error_occured();
        return -1;
    }
    
    // 1. Splitting the input by '&'
    char * commands[20];
    int num_of_commands = split_str(DELIM_PARALLEL, redirects[0], commands); // This gives the number of parallel commands
    if (num_of_commands > 1) SHOULD_APPEND = 1;
    else SHOULD_APPEND = 0;
    
    // 2. Filter each command by removing spaces
    remove_spaces(num_of_commands, commands); // This removes leading and trailing spaces from all commands
    
    // 3. Split each parallel command by ' '
    int i=0;
    while (i<num_of_commands) {
        char * components[10];
        int num_of_comps = split_str(DELIM, commands[i], components);
        int is_builtin = 0;
        
        // Check if it is built in command
        is_builtin = is_builtin_command(components[0]);
        if (is_builtin == 1) {
            process_builtin_command(num_of_comps, components);
            break;
        }
        
        // Create a new process for each command and run it in there
        // if it is NOT a built in command
        if (fork() == 0 && is_builtin == 0) {
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

// Checks if the given command is a builtin command or not
int is_builtin_command (char * command) {
    if (strcmp(command, EXIT_BUILTIN_CMD) == 0 || strcmp(command, CD_BUILTIN_CMD) == 0 || strcmp(command, PATH_BUILTIN_CMD) == 0) {
        return 1;
    }
    return 0;
}

// This executes the builtin commands in the parent process
int process_builtin_command (int count, char * components[]) {
    if (strcmp(components[0], EXIT_BUILTIN_CMD) == 0) { // Builtin Command - exit the dash
        if (count == 1) { // exit MUST have only 0 argument
            printf("Good Bye!\n\n");
            exit(0);
            return 0;
        }
        else {
            error_occured();
            return -1;
        }
    }
    else if (strcmp(components[0], CD_BUILTIN_CMD) == 0) { // Builtin Command - change the current directory
        if (count != 2) { // cd MUST have only 1 argument
            error_occured();
            return -1;
        }
        chdir(components[1]);
        if (errno != 0) error_occured();
        return 0;
    }
    else if (strcmp(components[0], PATH_BUILTIN_CMD) == 0) { // Builtin Command - change the path of the executables
        if (fork() == 0) {
            run_command(PATH_CMD_PATH, components, 0);
        }
        else {
            wait(NULL);
        }
        return 0;
    }
    return -1;
}

// This processes a command by checking if it can be
// run or not. If it can be run, it gets the path
// from where the command can be run.
// Each command runs in its own child process.
int process_command (int count, char * components[]) {
    // Before proceeding to run the command, check for the exec file
    char * exec_file_path;
    exec_file_path = (char *) malloc(30*sizeof(char));
    strcpy(exec_file_path, check_command(components[0]));
    if (strcmp(exec_file_path, EXECUTABLE_NOT_FOUND) == 0) {
        error_occured();
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
    int i=0;
    char * paths[20];
    FILE * file;
    
    // Read all paths from the list
    file = fopen(EXEC_FILE_PATH, "r+");
    if (errno == 2) {error_occured(); return EXECUTABLE_NOT_FOUND;}
    paths[i] = (char *) malloc(50*sizeof(char));
    fgets(paths[i], 32, file);
    while (strcmp(paths[i], "") != 0) {
        // For every path, append the command
        paths[i] = strtok(paths[i], "\n");
        strcat(paths[i], "/");
	strcat(paths[i], cmd);
        // For every path, check if the file exists
        if (access(paths[i], X_OK) == 0) {
            fclose(file);
            return paths[i];
        }
        i++;
        paths[i] = (char *) malloc(50*sizeof(char));
        fgets(paths[i], 32, file);
    }
    fclose(file);
    return EXECUTABLE_NOT_FOUND;
}

// Takes the path of the executable command
// along with the arguments. Runs the command.
void run_command (char * path, char * args[], int outputTarget) {
    // Check if output has to be redirected to another file
    if (OUTPUT_PATH != NULL) { // It means the output must go into a file
        if (SHOULD_APPEND == 1) {
            freopen(OUTPUT_PATH, "a+", stdout);
            freopen(OUTPUT_PATH, "a+", stderr);
        }
        else {
            freopen(OUTPUT_PATH, "w+", stdout);
            freopen(OUTPUT_PATH, "w+", stderr);
        }
    }
    else {
        freopen("/dev/tty", "w+", stdout);
        freopen("/dev/tty", "w+", stderr);
    }
    int res = execv(path, args);
    // Below code executes only if execv() fails
    freopen("/dev/tty", "w+", stdout);
    freopen("/dev/tty", "w+", stderr);
    error_occured();
    exit(res);
}

// Takes a string and splits it using @delim and puts each
// token in comps[]
int split_str(const char * delim, char * input, char * comps[]) {
    int i=0;
    comps[i] = strtok(input, delim);
    while (comps[i] != 0) {
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
        i++;
    }
}

// Returns 1 if multiple file names are given after redirection symbol
int has_multiple_files (char * file_part) {
    int len = strlen(file_part);
    int i=0;
    while (i<len) {
        if (*(file_part+i) == ' ') return 1;
        i++;
    }
    return 0;
}
