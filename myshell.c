#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

/*
Likhon D. Gomes
CIS 3207
Lab 2 - Shell
*/


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

void cd(char** args){
  if(chdir(args[1])!=0){
    printf("directory not found");
  }
}

void clr(){
  printf("\033[H\033[2J]");
}

void dir(char** args){
  DIR *dir;
  struct dirent *contents;
  if(args[1] == NULL){
    dir = opendir("./");
  } else {
    //check if the directory exists.
    if((dir = opendir(args[1]))==NULL){
      printf("No such directory.\n");
    }
  }

  while((contents = readdir(dir)) != NULL){
    printf("%s ", contents->d_name);
  }
  printf("\n");
  closedir(dir);
}

void environ(){

}

void echo(char** args){
  int i = 1;
  while(args[i] != NULL){
    printf("%s ",args[i]);
    args[i] = NULL;
    i++;
  }
  printf("\n");
}

void help(){

}


void execute(char **args){

  int runBG = 0; // running in the background

  if(args[0] == NULL){
    //checking if the args list is empty
  }else{

    int i = 0;
    while(args[i]!=NULL){
    
      //testing out the special symbols
    if(strcmp(args[0],">")==0){

    }
    if(strcmp(args[0],"<")==0){

    }
    if(strcmp(args[0],"&")==0){

    }
    if(strcmp(args[0],">>")==0){

    }
    if(strcmp(args[0],"|")==0){

    }


    //checking for internal functions
      if(strcmp(args[0],"exit")==0){
        quit();
      } else if (strcmp(args[0],"clear")==0){
        clr();
      } else if (strcmp(args[0],"echo")==0){
        echo(args);
      }else if (strcmp(args[0],"cd")==0){
        cd(args);
      } else if (strcmp(args[0],"dir")==0){
        dir(args);
      } else if (strcmp(args[0],"environ")==0){
        environ();
      } else if (strcmp(args[0],"help")==0){
        help();
      } else {
      //not a builtin function
      pid_t pid = fork();
        if(pid < 0){
          printf("fork failed");
        } else if ( pid == 0){
          execvp(args[0], args);
          printf("command or executable file not recognized\n");
        } else {
          if(runBG == 0){
            waitpid(pid, NULL,0);
          }
      }
      }
      i++;
    }
    }
  }

  

//This is the main function
int main(int argc, char *argv[]){
  printf("\033[H\033[2J]");

  while (loop != -1) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s>>mySHELL$ ", cwd);
    char* userInput = readLine();
    char** inputArray = parse(userInput);
    //printf("%s",inputArray[0]);
    execute(inputArray);
  }
}
