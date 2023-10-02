#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define BUFLEN 1024

// To Do: This base file has been provided to help you start the lab, you'll need to heavily modify it to implement all of the features

int main()
{
    char buffer[1024];
    char *parsedinput;
    char *args[3];
    char newline;

    printf("Welcome to the GroupXX shell! Enter commands, enter 'quit' to exit\n");
    do
    {
        // Print the terminal prompt and get input
        printf("$ ");
        char *input = fgets(buffer, sizeof(buffer), stdin);
        if (!input)
        {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }

        // Clean and parse the input string
        parsedinput = (char *)malloc(BUFLEN * sizeof(char));
        size_t parselength = trimstring(parsedinput, input, BUFLEN);

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

            char *token = strtok(parsedinput, " ");
            char *args[BUFLEN / 2]; // Assuming a reasonable maximum number of arguments
            int arg_count = 0;
            while (token != NULL && arg_count < BUFLEN / 2)
            {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }
            args[arg_count] = NULL; // Terminate the argument list with NULL

            for (int i = 0; i < arg_count; i++)
            {
                removeQuotes(args[i]);
            }

            pid_t forkV = fork();
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
            }
            else
                wait(NULL);
        }

        // Remember to free any memory you allocate!
        free(parsedinput);
    } while (1);

    return 0;
}

// size_t trimstring(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
// {
//     memcpy(outputbuffer, inputbuffer, bufferlen * sizeof(char));

//     for (size_t ii = strlen(outputbuffer) - 1; ii >= 0; ii--)
//     {
//         if (outputbuffer[ii] < '!') // In ASCII '!' is the first printable (non-control) character
//         {
//             outputbuffer[ii] = 0;
//         }
//         else
//         {
//             break;
//         }
//     }

//     return strlen(outputbuffer);
// }