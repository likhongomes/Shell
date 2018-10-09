#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const int bufferSize = 1024;
int loop = 0;

char* readLine(){
  char *input = malloc(sizeof(char)*bufferSize);
  char c;
  int pos = 0;
  while((c= getchar()) != '\n' && c != EOF){
    input[pos] = c;
    pos ++;
    if(pos >= bufferSize - 1) {
  		printf("User input too long. Please input args less than 1024 characters.\n");
  		break;
  	}
  }
  input[pos] = '\0';
  return input;
}

//This is the main function
int main(int argc, char *argv[]){
  system("clear");

  while (loop != -1) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s>>mySHELL$", cwd);
    char* userInput = readLine();
    printf("%s", userInput);
  }
}
