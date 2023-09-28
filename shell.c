#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define BUFLEN 1024

int main() {
    char buffer[1024];
    char* parsedinput;
    char* args[2];
    char newline;

    printf("Welcome to the Group06 shell! Enter commands, enter 'quit' to exit\n");
    do {
        // Print the terminal prompt and get input
        printf("$ ");
        char *input = fgets(buffer, sizeof(buffer), stdin);
        if (!input) {
            fprintf(stderr, "Error reading input\n");
            return -1;
        }

        // Clean and parse the input string
        parsedinput = (char*) malloc(BUFLEN * sizeof(char));
        size_t parselength = trimstring(parsedinput, input, BUFLEN);

        // Sample shell logic implementation
        if (strcmp(parsedinput, "quit") == 0) {
            printf("Bye!!\n");
            free(parsedinput);  // Free allocated memory before exiting
            return 0;
        } else {
            // Fork and execute the command
            pid_t forkV = fork();
            if (forkV == 0) {
                args[0] = parsedinput;  // Single-word command with its full path
                args[1] = NULL;  // Terminate the args array with NULL
                if (execve(parsedinput, args, NULL) == -1) {
                    fprintf(stderr, "Error running command in execve\n");
                    return -100;
                }
            } else {
                wait(NULL);
            }
        }

        // Remember to free any memory you allocate
        free(parsedinput);
    } while (1);

    return 0;
}
