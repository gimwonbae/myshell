#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024
#define WORD 128

void fatal(const char *str, int errcode){
  perror(str);
  exit(errcode);
}

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

int isBackground(char* buffer){
  for (int i = 0; i < strlen(buffer); i++){
    if(buffer[i] == '&'){
      buffer[i] = ' ';
      return 1;
    }
  }
  return 0;
}

void run(char* arg[], int background){
  pid_t pid;
  int status;

  pid = fork();
  if(pid == -1){
    fatal("fork error",1);
  }
  else if(pid == 0){
    execvp(arg[0],arg);
  }
  else{
    if(background == 0){
      wait(&status);
    }
    else {
      waitpid(pid,&status,WNOHANG);
    }
  }
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

    background = isBackground(buffer);

    argn = parsing(buffer, arg, argn);

    for(int i = 0; i < argn; i++){
      printf("arg[%d] : %s\n", i, arg[i]);
    }

    if (argn == 0){
      continue;
    }

    run(arg, background);
    // printf("%d\n", background);
  }
  return 0;
}