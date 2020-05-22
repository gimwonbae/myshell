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

int parsing(char buffer[], char* delimiter, char* arg[], int argn){
  char* ptr = strtok(buffer, delimiter);

  while (ptr != NULL && argn < WORD){
    arg[argn] = ptr;
    // printf("arg[%d] : %s\n", argn, arg[argn]);
    argn++;
    ptr = strtok(NULL, delimiter);
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

void cdCommand(char* arg[], int argn){
  // printf("argn : %d \n", argn);
  if(argn == 1){
    chdir(getenv("HOME"));
  }
  else if(argn == 2){
    if(chdir(arg[1]) == -1){
      printf("%s : No such file or directory\n", arg[1]);
    }
  }
  else{
    printf("too many argument\n");
  }
}

void historyCmd(char* arg[], int argn, char* history[]){
  if(argn == 1){
    for(int i = 0; history[i] != NULL; i++){
      printf(" %d %s", i+1, history[i]);
    }
  }
  else{
    printf("too many argument");
  }
}

void run(char* arg[], int background, int argn, char* history[]){
  pid_t pid;
  int status;

  if(strcmp(arg[0],"cd") == 0){
    cdCommand(arg, argn);
  }

  else if(strcmp(arg[0],"history") == 0){
    historyCmd(arg, argn, history);
  }

  else{
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
}

void multiCmd(char buffer[], int background, char* history[]){
  char* arg[WORD];
  char* newArg[WORD];

  memset(arg,'\0',sizeof(arg));
  memset(newArg,'\0',sizeof(arg));

  int argn = 0;
  int newArgn = 0;

  newArgn = parsing(buffer, "\t\r\n;", newArg, newArgn);
  // for(int i = 0; i < newArgn; i++){
  // printf("newarg[%d] : %s, newargn = %d\n", i, newArg[i], newArgn);
  // }
  for(int i = 0; i < newArgn; i++){
    memset(arg,'\0',sizeof(arg));
    argn = 0;
    argn = parsing(newArg[i], " \t\r\n", arg, argn);
    run(arg, background, argn, history);
  }
}

void loop(char buffer[], int background, char* history[]){
  char* arg[WORD];
  char* newArg[WORD];

  memset(arg,'\0',sizeof(arg));
  memset(newArg,'\0',sizeof(arg));

  int argn = 0;
  int newArgn = 0;
  if(strchr(buffer, ';')){
    multiCmd(buffer,background,history);
  }

  else{
    argn = parsing(buffer, " \t\r\n", arg, argn);
    run(arg, background, argn, history);
  }
}

int main(void) {
  char buffer[BUFSIZE];
  char buffer2[BUFSIZE];
  char *history[BUFSIZE];
  char* arg[WORD];
  int historyCnt = 0;

  while (1) {
    int background = 0;
    int argn = 0;

    memset(buffer,'\0',sizeof(buffer));
    memset(arg,'\0',sizeof(arg));
    // memset(history,'\0',sizeof(history));

    printf("%s$ ",getcwd(NULL, BUFSIZE));
    // fflush(stdout);
    fgets(buffer, BUFSIZE, stdin);
    
    strcpy(buffer2,buffer);
    argn = parsing(buffer2," \t\r\n", arg, argn);
    if (argn == 0){
      continue;
    }

    history[historyCnt] = (char *)malloc(sizeof(char) * strlen(buffer) + 1);
    memset(history[historyCnt],'\0',(sizeof(char) * strlen(buffer) + 1));
    history[historyCnt+1] = NULL;
    
    strcpy(history[historyCnt], buffer);
    // printf("history[%d] = %s", historyCnt, history[historyCnt]);
    historyCnt++;

    background = isBackground(buffer);
    
    // for(int i = 0; i < argn; i++){
    //   printf("arg[%d] : %s\n", i, arg[i]);
    // }
    loop(buffer,background,history);

    // printf("%d\n", background);
  }
  return 0;
}