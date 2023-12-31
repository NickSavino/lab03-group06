#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define BUFLEN 1024
#define MAX_BG_PROCESSES 100

//tracks running background processes
pid_t bg_pids[MAX_BG_PROCESSES];
int bg_count = 0;


//forward declarations
void preparePipeCommand(char *command1[], char *command2[]);
int prepareCommand(char* args[]);
void handleCD(char* args[]);
int runCommand(char* args[]);
void checkBackgroundProcesses();

int background = 0; // Flag
int main()
{
    char buffer[1024];
    char *parsedinput;
    char *args[BUFLEN / 2];
    char *firstWord;

    printf("Welcome to the Group06 shell! Enter commands, enter 'quit' to exit\n");
    do
    {
        // Print the terminal prompt and get input
        char cwd[BUFLEN];
        if (getcwd(cwd, sizeof(cwd)))
        {
            printf("%s$ ", cwd);
        }
        else
        {
            printf("$ ");
        }
        char *input = fgets(buffer, sizeof(buffer), stdin);
        if (!input)
        {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }
        //handles empty inputs
        if (strcmp(input, "\n") == 0)
        {
            continue;
        }
        // Clean and parse the input string
        parsedinput = (char *)malloc(BUFLEN * sizeof(char));
        size_t parselength = trimstring(parsedinput, input, BUFLEN);

        firstWord = (char *)malloc(BUFLEN * sizeof(char));
        size_t firstWordLength = firstword(firstWord, parsedinput, BUFLEN);

        // Sample shell logic implementation
        if (strcmp(parsedinput, "quit") == 0)
        {
            printf("Bye!!\n");
            return 0;
        }
        else
        {

            int pipeLocation = findpipe(parsedinput, BUFLEN);
            if (pipeLocation != -1) {
                char* firstCommand = strtok(parsedinput, "|");
                char* secondCommand = strtok(NULL, "|");
                char* args1[BUFLEN / 2];
                char* args2[BUFLEN / 2];
                tokenizeInput(args1, firstCommand, BUFLEN);
                tokenizeInput(args2, secondCommand, BUFLEN);
                preparePipeCommand(args1, args2);
            } else {
            //Converts the input into a set of tokens
            tokenizeInput(args, parsedinput, BUFLEN);

        int arg_count = 0;
        while (args[arg_count] != NULL) {
            arg_count++;
        }

        if (arg_count > 0 && strcmp(args[arg_count - 1], "&") == 0) {
            background = 1;
            args[arg_count - 1] = NULL; // Remove the '&' symbol
        }
            //Handles cd command
            //TODO move to separate function
            if (strcmp(firstWord, "cd") == 0) {
                handleCD(args);
                free(firstWord);
                free(parsedinput);
                continue;
            }
            prepareCommand(args);
            
            }   
            
        }
        // Remember to free any memory you allocate!
        free(firstWord);
        free(parsedinput);
        background = 0;
        checkBackgroundProcesses();
    } while (1);

    return 0;
    }

//allows shell to utilize "cd" command
void handleCD(char* args[]) {
        char *path = args[1];
        if (path == NULL) {
            //sets path to default if no argument is supplied
            path = getenv("HOME");
        }
        if (chdir(path) != 0) {
            perror("cd");
        }
}

int prepareCommand(char* args[]) {

    //create child process
    pid_t forkV = fork();
    if (forkV == -1) {
        perror("fork");
        return -1;
    }
    if (forkV == 0) // if successful, execute command
    {
        runCommand(args);
    } else
        //if background flag is 1, add to list of background processes
        if (background) {
            if (bg_count < MAX_BG_PROCESSES) {
                bg_pids[bg_count++] = forkV;
            } else {
            waitpid(forkV, NULL, 0);
            }
        }
    return 0;
}

void preparePipeCommand(char *command1[], char *command2[]) {
    int pipe_fd[2]; // File descriptors for the pipe

    // Create the pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork the first process
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(pipe_fd[0]); // Close read end
        dup2(pipe_fd[1], STDOUT_FILENO); // Redirect standard output to write end of the pipe
        close(pipe_fd[1]);
        runCommand(command1);
        exit(0);
    }

    // Fork the second process
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipe_fd[1]); // Close write end
        dup2(pipe_fd[0], STDIN_FILENO); // Redirect standard input to read end of the pipe
        close(pipe_fd[0]);
        runCommand(command2); // Execute the second command
        exit(0);
    }

    // Parent process
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(pid2, NULL, 0);
}


//executes prepares commands
int runCommand(char* args[]) {
          if (args[0][0] == '/')
        {
            if (execve(args[0], args, NULL) == -1)
            {
                perror("Error executing absolute path");
                return -100;
            }
        }
        else if (strchr(args[0], '/') != NULL)
        {
            // Execute as a relative path in the current working directory
            if (execve(args[0], args, NULL) == -1)
            {
                perror("Error executing relative path");
                return -100;
            }
        }
        else
        {
            // Check if the command is in the PATH
            char *path_env = getenv("PATH");
            if (path_env != NULL)
            {
                char *path = strdup(path_env);
                char *token = strtok(path, ":");
                while (token != NULL)
                {
                    char full_path[BUFLEN];
                    snprintf(full_path, sizeof(full_path), "%s/%s", token, args[0]);
                    if (access(full_path, X_OK) == 0)
                    {
                        execve(full_path, args, NULL) == -1;
                    }
                    token = strtok(NULL, ":");
                }
                free(path);
            }
            // If the command was not found in PATH
            fprintf(stderr, "Command not found: %s\n", args[0]);
            perror(args[0]);
            exit(EXIT_FAILURE);
        }
}


//checks for any background processes running
void checkBackgroundProcesses() {
    int status;
    pid_t terminated_pid;
    while ((terminated_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < bg_count; i++) {
            if (bg_pids[i] == terminated_pid) {
                printf("Background command %d terminated\n", terminated_pid);
                // Remove the PID from the array
                for (int j = i; j < bg_count - 1; j++) {
                    bg_pids[j] = bg_pids[j + 1];
                }
                bg_count--;
                break;
            }
        }
    }
}
