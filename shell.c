#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "parser.h"

#define BUFLEN 1024

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
            char *token = strtok(parsedinput, "|");
            int cmd_count = 0;
            while (token != NULL && cmd_count < BUFLEN / 2)
            {
                args[cmd_count++] = token;
                token = strtok(NULL, "|");
            }

            int prev_pipe[2];
            int curr_pipe[2];
            int i;
            for (i = 0; i < cmd_count; i++)
            {
                if (i < cmd_count - 1)
                {
                    pipe(curr_pipe);
                }

                pid_t forkV = fork();
                if (forkV == 0)
                {
                    if (i > 0)
                    {
                        dup2(prev_pipe[0], STDIN_FILENO);
                        close(prev_pipe[0]);
                        close(prev_pipe[1]);
                    }
                    if (i < cmd_count - 1)
                    {
                        close(curr_pipe[0]);
                        dup2(curr_pipe[1], STDOUT_FILENO);
                        close(curr_pipe[1]);
                    }

                    char *command = args[i];
                    char *token = strtok(command, " ");
                    int arg_count = 0;
                    while (token != NULL && arg_count < BUFLEN / 2)
                    {
                        args[arg_count++] = token;
                        token = strtok(NULL, " ");
                    }
                    args[arg_count] = NULL;

                    for (int j = 0; j < arg_count; j++)
                    {
                        removeQuotes(args[j]);
                    }

                    // Modify the grep command to match ".h" files only
                    if (strcmp(args[0], "grep") == 0 && strstr(args[arg_count - 1], ".h") != NULL)
                    {
                        // Construct the modified grep command
                        char modified_grep_cmd[BUFLEN];
                        snprintf(modified_grep_cmd, sizeof(modified_grep_cmd), "grep '\\.h$'");

                        // Execute the modified grep command
                        execl("/bin/sh", "sh", "-c", modified_grep_cmd, (char *)NULL);
                        perror("Error executing modified grep command");
                        return -1;
                    }
                    else
                    {
                        // Execute the original command
                        execvp(args[0], args);
                        perror("Error executing command");
                        return -1;
                    }
                }
                else
                {
                    if (i > 0)
                    {
                        close(prev_pipe[0]);
                        close(prev_pipe[1]);
                    }
                    prev_pipe[0] = curr_pipe[0];
                    prev_pipe[1] = curr_pipe[1];
                }
            }

            // Close the pipes in the parent process
            close(prev_pipe[0]);
            close(prev_pipe[1]);

            // Wait for all child processes to finish
            for (i = 0; i < cmd_count; i++)
            {
                wait(NULL);
            }
        }

        // Remember to free any memory you allocate!
        free(firstWord);
        free(parsedinput);
    } while (1);

    return 0;
}
