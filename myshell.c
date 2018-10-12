#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

char** parse(char* input){
  char* space = " ";
  char *arg = strtok(input,space);
  char** argsArray = malloc(sizeof(char)*bufferSize);
  int i = 0;

  while(arg != NULL){
    argsArray[i] = arg;
    i++;
    arg = strtok(NULL, space);
  }
  argsArray[i] = NULL;
  return argsArray;
}

void quit(){
  loop = -1;
}

void cd(){

}

void clr(){
  system("clear");
}

void dir(){

}

void environ(){

}

void echo(){

}

void help(){

}


void execute(char **args){
  if(strcmp(inputArray[0],"exit")==0){
      quit();
    } else if (strcmp(inputArray[0],"clear")==0){
      clr();
    }
}

//This is the main function
int main(int argc, char *argv[]){
  system("clear");

  while (loop != -1) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s>>mySHELL$ ", cwd);
    char* userInput = readLine();
    char** inputArray = parse(userInput);
    //printf("%s",inputArray[0]);
    execute(inputUser);
  }
}
