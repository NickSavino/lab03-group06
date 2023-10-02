#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include "parser.h"

#define BUFLEN 1024

int main() {
    printf("Testing Parser Implementation\n");

    //To Do: These are just some example test strings for your parser, you'll probably want to add to and modify them
    char teststring1[BUFLEN] = "command arg1 arg2";
    char teststring2[BUFLEN] = "This is a test string   \n";
    char teststring3[BUFLEN] = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0xED, 0x00};
    char teststring4[BUFLEN] = "test pipe | parsing\n";

    printf("Testing String Trimming\n");
    //First test string should be the same length after trimming
    char * testoutput1;
    testoutput1 = (char*) malloc(BUFLEN * sizeof(char));
    trimstring(testoutput1, teststring1, BUFLEN);
    assert(strlen(teststring1) == strlen(testoutput1));    

    //Second test string should be shorter after trimming
    char * testoutput2;
    testoutput2 = (char*) malloc(BUFLEN * sizeof(char));
    trimstring (testoutput2, teststring2, BUFLEN);
    assert(strlen(teststring2) > strlen(testoutput2));
    free(testoutput2);

    printf("Testing ASCII String Validation\n");
    //First test string should pass validation
    assert(isvalidascii(teststring1, BUFLEN) == true);

    //Second test string should fail validation
    assert(isvalidascii(teststring2, BUFLEN) == false);

    //Third test string should fail validation
    assert(isvalidascii(teststring3, BUFLEN) == false);

    printf("Testing firstword\n");
    char * testoutput3;
    testoutput3 = (char*) malloc(BUFLEN * sizeof(char));
    firstword(testoutput3, teststring1, BUFLEN);

    assert(strcmp(testoutput3, "command") == 0);

    printf("Testing findpipe\n");
    int testoutput4;
    testoutput4 = findpipe(teststring4, BUFLEN);
    assert(testoutput4 == 10);

    char teststring5[BUFLEN] = "ls -l &";
    char teststring6[BUFLEN] = "cd .. | grep 5 & ";
    char teststring7[BUFLEN] = "test command &\n";
    printf("Testing runinbackground\n");
    assert(runinbackground(teststring5, BUFLEN) == true);
    assert(runinbackground(teststring6, BUFLEN) == true);
    assert(runinbackground(teststring7, BUFLEN) == true);
    assert(runinbackground(teststring4, BUFLEN) == false);


    printf("All Tests Passed!\n");
    return 0;
}