#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFSIZE 1024
#define WORD 128

void fatal(const char *str, int errcode);
int parsing(char buffer[], char* delimiter, char* arg[], int argn);
int isBackground(char* buffer);
void cdCommand(char* arg[], int argn);
void historyCmd(char* arg[], int argn, char* history[]);
void historyNum(char* arg[], int argn, char* history[], int* historyCnt);
int run(char* arg[], int background, int argn, char* history[],  int* historyCnt);
void multiCmd(char buffer[], int background, char* history[], int* historyCnt);
void loop(char buffer[], char* history[], int* historyCnt);
void isRedirect(char* arg[], int argn);

void fatal(const char *str, int errcode){
  perror(str);
  exit(errcode);
}

int parsing(char buffer[], char* delimiter, char* arg[], int argn){
  char* ptr = strtok(buffer, delimiter);

  while (ptr != NULL && argn < WORD){
    arg[argn] = ptr;
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

void historyNum(char* arg[], int argn, char* history[], int* historyCnt){
  if(argn == 1){
    arg[0]++;
    if(atoi(arg[0]) != 0){
      (*historyCnt)--;
      if(atoi(arg[0]) > *historyCnt){
        printf("not found\n");
      }
      else{
        char* record = (char *)malloc(sizeof(char) * strlen(history[atoi(arg[0])-1]) + 1);
        memset(record,'\0',(sizeof(char) * strlen(history[atoi(arg[0])-1]) + 1));
        strcpy(record,history[atoi(arg[0])-1]);
        loop(history[atoi(arg[0])-1], history, historyCnt);
        strcpy(history[atoi(arg[0])-1],record);
      }
    }
    else{
      printf("should !number \n");
    }
  }
  else{
    printf("too many argument");
  }
}

void isRedirect(char* arg[], int argn){
  int fd;
  for(int i = 0; i < argn; i++){
    if(strcmp(arg[i],">")){
      if((fd = open(arg[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0){
        fatal("redirect open error", 1);
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
      arg[i] = NULL;
      break;
    }
    else if(strcmp(arg[i],">>")){
      if((fd = open(arg[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644)) < 0){
        fatal("redirect open error", 1);
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
      arg[i] = NULL;
      break;      
    }
    else if(strcmp(arg[i],"<")){
      if((fd = open(arg[i+1], O_WRONLY | O_CREAT, 0644)) < 0){
        fatal("redirect open error", 1);
      }
      dup2(fd, STDIN_FILENO);
      close(fd);
      arg[i] = NULL;
      break;      
    }
    else if(strcmp(arg[i],"<<")){
      
    }
    else if(strcmp(arg[i],"2>")){
      if((fd = open(arg[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0){
        fatal("redirect open error", 1);
      }
      dup2(fd, STDERR_FILENO);
      close(fd);
      arg[i] = NULL;
      break;      
    }
    else if(strcmp(arg[i],">|")){
      
    }
  }
}

int run(char* arg[], int background, int argn, char* history[],  int* historyCnt){
  pid_t pid;
  int status;
  char* ptr;
  if(strcmp(arg[0],"cd") == 0){
    cdCommand(arg, argn);
    return 0;
  }
  if((pid = fork()) == -1){
    fatal("fork error",1);
  }
  else if(pid == 0){
    if(strcmp(arg[0],"history") == 0){
      historyCmd(arg, argn, history);
    }
    else if(arg[0] == strchr(arg[0], '!')){
      historyNum(arg, argn, history, historyCnt);
    }
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
  return 0;
}

void multiCmd(char buffer[], int background, char* history[], int* historyCnt){
  char* arg[WORD];
  char* newArg[WORD];

  memset(arg,'\0',sizeof(arg));
  memset(newArg,'\0',sizeof(arg));

  int argn = 0;
  int newArgn = 0;

  newArgn = parsing(buffer, "\t\r\n;", newArg, newArgn);
  
  for(int i = 0; i < newArgn; i++){
    memset(arg,'\0',sizeof(arg));
    argn = 0;
    argn = parsing(newArg[i], " \t\r\n", arg, argn);
    run(arg, background, argn, history, historyCnt);
  }
}

void loop(char buffer[], char* history[], int* historyCnt){
  char* arg[WORD];

  int argn = 0;

  memset(arg,'\0',sizeof(arg));

  history[*historyCnt] = (char *)malloc(sizeof(char) * strlen(buffer) + 1);
  memset(history[*historyCnt],'\0',(sizeof(char) * strlen(buffer) + 1));
  history[*historyCnt+1] = NULL;
  strcpy(history[*historyCnt], buffer);

  (*historyCnt)++;

  int background = isBackground(buffer);

  if(strchr(buffer, ';')){
    multiCmd(buffer,background,history, historyCnt);
  }

  else{
    argn = parsing(buffer, " \t\r\n", arg, argn);
    run(arg, background, argn, history, historyCnt);
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
    
    printf("%s$ ",getcwd(NULL, BUFSIZE));
    fgets(buffer, BUFSIZE, stdin);
    
    strcpy(buffer2,buffer);
    argn = parsing(buffer2," \t\r\n", arg, argn);
    if (argn == 0){
      continue;
    }
  
    loop(buffer,history,&historyCnt);
  }
  return 0;
}