#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "arch/x86/include/generated/uapi/asm/unistd_64.h"
int main( int argc, char** argv){
    char *in = "This is a stupid message.";
  char msg[50];
  int msglen;

  printf("%d \n", syscall(__NR_messageboxget, msg, 50));
  printf("%s \n", msg);
}