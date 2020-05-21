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
      printf("%s : No such file or directory", arg[1]);
    }
  }
  else{
    printf("cd [directory name]\n");
  }
}

void historyCommand(){

}

void run(char* arg[], int background, int argn){
  pid_t pid;
  int status;

  if(strcmp(arg[0],"cd") == 0){
    cdCommand(arg, argn);
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

int main(void) {
  char buffer[BUFSIZE];
  char* arg[WORD];
  char* newArg[WORD];
  char* history[BUFSIZE];

  while (1) {
    int argn = 0;
    int newArgn = 0;
    int background = 0;

    memset(arg,'\0',sizeof(arg));
    memset(newArg,'\0',sizeof(arg));
    memset(buffer,'\0',sizeof(buffer));

    printf("%s$ ",getcwd(NULL, BUFSIZE));
    // fflush(stdout);
    fgets(buffer, BUFSIZE, stdin);

    background = isBackground(buffer);

    if(strchr(buffer, ';')){
      newArgn = parsing(buffer, ";", newArg, newArgn);
      // for(int i = 0; i < newArgn; i++){
      //   printf("newarg[%d] : %s, newargn = %d\n", i, newArg[i], newArgn);
      // }
      for(int i = 0; i < newArgn; i++){
        argn = 0;
        argn = parsing(newArg[i], " \t\r\n", arg, argn);
        run(arg, background, argn);
      }
    }

    else{
      argn = parsing(buffer, " \t\r\n", arg, argn);
      run(arg, background, argn);
    }
    
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