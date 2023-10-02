#include "parser.h"


// Command to trim whitespace and ASCII control characters from buffer
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming
//[Return] size_t - size of output string after trimming
size_t trimstring(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
{
    memcpy(outputbuffer, inputbuffer, bufferlen * sizeof(char));

    for (size_t ii = strlen(outputbuffer) - 1; ii >= 0; ii--)
    {
        if (outputbuffer[ii] < '!') // In ASCII '!' is the first printable (non-control) character
        {
            outputbuffer[ii] = 0;
        }
        else
        {
            break;
        }
    }

    return strlen(outputbuffer);
}

// Command to trim the input command to just be the first word
//[Input] char* inputbuffer - input string to trim
//[Input] size_t bufferlen - size of input and output string buffers
//[Output] char* outputbuffer - output string after trimming
//[Return] size_t - size of output string after trimming
size_t firstword(char *outputbuffer, const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Implement this function

    size_t i = 0;
    while (i < bufferlen && inputbuffer[i] != ' ' && inputbuffer[i] != '\0' && inputbuffer[i] != '\n') {
        outputbuffer[i] = inputbuffer[i];
        i++;
    }
    outputbuffer[i] = '\0';
    return i;
}

// Command to test that string only contains valid ascii characters (non-control and not extended)
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if no invalid ASCII characters present
bool isvalidascii(const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Correct this function so that the second test string fails
    size_t testlen = bufferlen;
    size_t stringlength = strlen(inputbuffer);
    if (strlen(inputbuffer) < bufferlen)
    {
        testlen = stringlength;
    }

    bool isValid = true;
    for (size_t ii = 0; ii < testlen; ii++)
    {
        isValid &= ((unsigned char)inputbuffer[ii] >= ' ' && (unsigned char)inputbuffer[ii] <= '~'); // In (lower) ASCII '~' is the last printable character
    }

    return isValid;
}

// Command to find location of pipe character in input string
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] int - location in the string of the pipe character, or -1 pipe character not found
int findpipe(const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Implement this function
    for (size_t i = 0; i < bufferlen; i++) {
        if (inputbuffer[i] == '|')
            return i;
    }
    return -1;
}

// Command to test whether the input string ends with "&" and
// thus represents a command that should be run in background
//[Input] char* inputbuffer - input string to test
//[Input] size_t bufferlen - size of input buffer
//[Return] bool - true if string ends with "&"
bool runinbackground(const char *inputbuffer, size_t bufferlen)
{
    // TO DO: Implement this function
    for (int i = strlen(inputbuffer) - 1; i >=0; i--) {
        if (inputbuffer[i] == ' ' || inputbuffer[i] == '\t' || inputbuffer[i] == '\n')
            continue;
        if (inputbuffer[i] == '&')
            return true;
    }
    return false;
}

void removeQuotes(char *str)
{
    int len = strlen(str);
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"')
    {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

void tokenizeInput(char* args[], char* parsedInput, size_t bufferlen) {
    char *token = strtok(parsedInput, " ");
    if (!args) {
        perror("Memory Allocation Failed");
        exit(EXIT_FAILURE);
    }

    int arg_count = 0;
    while (token != NULL && arg_count < bufferlen / 2)
    {
        args[arg_count++] = token;
        token = strtok(NULL, " ");
    }
    args[arg_count] = NULL; // Terminate the argument list with NULL

    for (int i = 0; i < arg_count; i++)
    {
        removeQuotes(args[i]);
    }

}