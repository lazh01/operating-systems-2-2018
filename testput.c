#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "arch/x86/include/generated/uapi/asm/unistd_64.h"
int main( int argc, char** argv){
  char *in = "Normal";
  char msg[50];
  int msglen;

  printf("%d \n", syscall(__NR_messageboxput, in, strlen(in)+1));
}