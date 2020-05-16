#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024
#define WORD 128

int parsing(char buffer[], char* arg[], int argn){
  char* ptr = strtok(buffer, " \t\r\n");

  while (ptr != NULL && argn < WORD){
    arg[argn] = ptr;
    // printf("arg[%d] : %s\n", argn, arg[argn]);
    argn++;
    ptr = strtok(NULL, " \t\r\n");
  }

  return argn;
}

int isBackground(char* arg[], int argn){
  int background = 0;
  for (int i = 0; i < argn; i++){
    for(int j = 0; j < strlen(arg[i]); j++){
      if (arg[i][j] == '&'){
        background = 1;
        arg[i][j] = '\0';
      }
    }
  }
  return background;
}

int main(void) {
  char buffer[BUFSIZE];
  char* arg[WORD];

  while (1) {
    int argn = 0;
    int background = 0;

    memset(arg,'\0',sizeof(arg));
    memset(buffer,'\0',sizeof(buffer));

    printf("smsh> ");
    fflush(stdout);
    fgets(buffer, BUFSIZE, stdin);

    argn = parsing(buffer, arg, argn);

    background = isBackground(arg, argn);

    // for(int i = 0; i < argn; i++){
    //   printf("arg[%d] : %s\n", i, arg[i]);
    // }

    if (argn == 0){
      continue;
    }

    // printf("%d\n", background);
  }
  return 0;
}