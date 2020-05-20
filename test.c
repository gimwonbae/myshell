#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void eliminate(char *argChild, char target)
{
    for (; *argChild != '\0'; argChild++)
    {
        if (*argChild == target)
        {
            strcpy(argChild, argChild + 1);
            argChild--;
        }
    }
}

int main(){
  int max = 1024;
  char* arg[max];
  char a[16] = "av";
	eliminate(a, 'a');
	printf("%s", a);
	return 0;
}