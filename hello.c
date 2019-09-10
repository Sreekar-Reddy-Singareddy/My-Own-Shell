#include <stdio.h>
#include <string.h>

int main () {
  char ins[10][100];
  char s[100];
  printf("Enter> ");
  scanf("%s",&s);
  while(strcmp(s,"\n") != 0){
    printf("Input Given::: %s\n",s);
    scanf("%s",&s);
  }
  return 0;
}
