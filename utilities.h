#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

const int bufferSize = 1024;
int loop = 0;
int dbm = 0;

//this is where all the execution happens
char* readLine();
char** parse(char* input);
void quit();
void cd(char** args);
void clr();
void dir(char** args);
void environ();
void echo(char** args);
void help(char** args);
int makePipe(char** args1, char** args2);
void halt();
void execute(char **args);
void singleCommandMode();
void batchExecution(char filename[100]);

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

//this is the parse function
char** parse(char* input){
  char* space = " ";
  char *arg = strtok(input,space);
  char** argsArray = malloc(sizeof(char)*bufferSize);
  int i = 0;

  //looping through all the words in the command.
  while(arg != NULL){
    argsArray[i] = arg;
    i++;
    arg = strtok(NULL, space);
  }
  argsArray[i] = NULL;
  return argsArray;
}