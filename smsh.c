#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024
#define WORD 128

int arrayShift(char* arg[], char* newarg[], int size, int num){
  for(int i =0; i < size-num; i++){
    newarg[i] = arg[i+num];
  }
  arg[num] = NULL;
  newarg[size-num] = NULL;
  return size-num;
}

void fatal(const char *str, int errcode){
  perror(str);
  exit(errcode);
}

int parsing(char buffer[], char* condition, char* arg[], int argn){
  char* ptr = strtok(buffer, condition);

  while (ptr != NULL && argn < WORD){
    arg[argn] = ptr;
    // printf("arg[%d] : %s\n", argn, arg[argn]);
    argn++;
    ptr = strtok(NULL, condition);
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
  char* newBuffer[BUFSIZE];

  while (1) {
    int argn = 0;
    int background = 0;

    memset(arg,'\0',sizeof(arg));
    memset(buffer,'\0',sizeof(buffer));

    printf("smsh> ");
    fflush(stdout);
    fgets(buffer, BUFSIZE, stdin);

    background = isBackground(buffer);

    argn = parsing(buffer, ' /t/s/n', arg, argn);

    // for(int i = 0; i < argn; i++){
    //   printf("arg[%d] : %s\n", i, arg[i]);
    // }

    if (argn == 0){
      continue;
    }
    for (int i = 0; i < argn; i++){
      if(strchr(arg[i], ';')){
        // int cknum = arrayShift(arg,newarg,argn,i+1);
        // newBuffer = parsing();
        run(arg, background);
        //memcpy(arg, newarg, cknum-1);
        // arg[cknum] = NULL;
      }
      else{
        run(arg, background);
      }
    }

    // printf("%d\n", background);
  }
  return 0;
}