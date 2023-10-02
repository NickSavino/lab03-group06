#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define BUFLEN 1024

void executePipeCommand(char *command1[], char *command2[]);
int executeCommand(char* args[]);
void handleCD(char* args[]);


// To Do: This base file has been provided to help you start the lab, you'll need to heavily modify it to implement all of the features

int main()
{
    char buffer[1024];
    char *parsedinput;
    char *args[3];
    char newline;
    char *firstWord;

    printf("Welcome to the Group06 shell! Enter commands, enter 'quit' to exit\n");
    do
    {
        // Print the terminal prompt and get input
        char cwd[BUFLEN];
        if (getcwd(cwd, sizeof(cwd))) {
            printf("%s$ ", cwd);
        } else {
            printf("$ ");
        }
        char *input = fgets(buffer, sizeof(buffer), stdin);
        if (!input)
        {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }
        //handles empty inputs
        if (strcmp(input, "\n") == 0) {
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
            // so once fork is called, inside the child proccess, the fork is == 0, so the
            // if statement runs

            // however, in the parent proccess, the fork is not == to 0, so
            // the else statement runs, and waits for the child proccess
            
            //pip file descriptor
            int pipe_fd[2];
            int pipeLocation = findpipe(parsedinput, BUFLEN);
            if (pipeLocation != -1) {
                char* firstCommand = strtok(parsedinput, "|");
                char* secondCommand = strtok(NULL, "|");
                char* args1[BUFLEN / 2];
                char* args2[BUFLEN / 2];
                tokenizeInput(args1, firstCommand, BUFLEN);
                tokenizeInput(args2, secondCommand, BUFLEN);
                executePipeCommand(args1, args2);
            } else {
            //Converts the input into a set of tokens
            tokenizeInput(args, parsedinput, BUFLEN);

            //Handles cd command
            //TODO move to separate function
            if (strcmp(firstWord, "cd") == 0) {
                handleCD(args);
                free(firstWord);
                free(parsedinput);
                continue;
            }
            executeCommand(args);
            
            }   
        }
        // Remember to free any memory you allocate!
        free(firstWord);
        free(parsedinput);
    } while (1);

    return 0;
    }

int executeCommand(char* args[]) {

    pid_t forkV = fork();
    if (forkV == -1) {
        perror("fork");
        return -1;
    }
    if (forkV == 0)
    {
        // pasredinput = exaclty what is typed into cmd

        // printf("\n%s\n", args[1]);
        // for features one and 2, need /usr/bin/<command> <paramters> to run

        // execve(cont char *path, cont char*arg[],cont char* enviroment[]);
        // Check if the command is an absolute path
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
                        if (execve(full_path, args, NULL) == -1)
                        {
                            perror("Error executing command from PATH");
                            return -100;
                        }
                    }
                    token = strtok(NULL, ":");
                }
                free(path);
            }
            // If the command was not found in PATH
            fprintf(stderr, "Command not found: %s\n", args[0]);
            return -1;
        }
    } else
        wait(NULL);
    return 0;
}

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

void executePipeCommand(char *command1[], char *command2[]) {
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
        executeCommand(command1);
        perror("execvp"); // Only reached if execvp fails
        exit(EXIT_FAILURE);
    }

    // Fork the second process
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipe_fd[1]); // Close write end
        dup2(pipe_fd[0], STDIN_FILENO); // Redirect standard input to read end of the pipe
        close(pipe_fd[0]);
        executeCommand(command2); // Execute the second command
        perror("execvp"); // Only reached if execvp fails
        exit(EXIT_FAILURE);
    }

    // Parent process
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}