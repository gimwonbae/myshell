#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024
#define WORD 128

int main(void) {
  char buffer[BUFSIZE];
  char* arg[WORD];
  
  int background = 0;

  while (1) {
    int argn = 0;
    memset(arg,'\0',sizeof(arg));
    memset(buffer,'\0',sizeof(buffer));
  
    printf("smsh> ");
    fflush(stdout);
    fgets(buffer, BUFSIZE, stdin);

    char* ptr = strtok(buffer, " \t\r\n");

    while (ptr != NULL && argn < WORD){
      arg[argn] = ptr;
      printf("arg[%d] : %s\n", argn, arg[argn]);
      argn++;
      ptr = strtok(NULL, " \t\r\n");
    }
    
    for (int i = 0; i < argn; i++){
      for(int j = 0; j < strlen(arg[i]); j++){
        if (arg[i][j] == '&'){
          background = 1;
          arg[i][j] = '\0';
        }
      }
    }

    for(int i = 0; i < argn; i++){
      printf("arg[%d] : %s\n", i, arg[i]);
    }

    if (argn == 0){
      continue;
    }

    printf("%d\n", background);
  }
  return 0;
}