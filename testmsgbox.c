#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "arch/x86/include/generated/uapi/asm/unistd_64.h"

void testInputOutputMatch(){
    char *in = "test";
    char out[strlen(in)+1];
    int errno;
    errno = syscall(__NR_messageboxput, in, strlen(in)+1);
    if (errno == 0){
        printf("Succesfully added message to message box\n");
    } else {
        printf("Failed to add message to message box\n");
    }
    syscall(__NR_messageboxget, out, strlen(in)+1);
    if (strcmp(in, out) == 0){
        printf("passed test 1, inserted string is the same as output\n");
    } else {
        printf("error: output is not identical to output\n");
        printf("%s\n", in);
        printf("%s\n", out);
    }
}

void testLengthArgument(){
    char *in = "test";
    int errno;
    errno = syscall(__NR_messageboxput, in, -3);

    if (errno == -22){
        printf("Succesfully returned error when trying to add negative length\n");
    } else {
        printf("Failed to return error on negative length\n");
    }
}

void testGetSmallBuffer(){
    char *in = "test";
    char out[1];
    int errno;
    errno = syscall(__NR_messageboxput, in, strlen(in) + 1);
    errno = syscall(__NR_messageboxget, out, 1);
    if(errno == -22){
        printf("Succesfully denied get with a buffer smaller than input\n");
    } else {
        printf("error: allowed getting message into a smaller buffer\n");
    }
}


int main( int argc, char** argv){
  testInputOutputMatch();
  testLengthArgument();
  testGetSmallBuffer();
}

