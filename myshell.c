#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/ioctl.h>

#include <signal.h>

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
  int numOfVars = 7;
  char* info[] = {"USER","HOME","PATH","SHELL","OSTYPE","PWD","GROUP"};

  int i;
  for(i = 0; i < numOfVars; i++){
    const char* var = getenv(info[i]);
    if(var != NULL){
      printf("%s: %s\n",info[i], var);
    } else {
      printf("Error: Could not fetch the environment variables.");
    }
  }
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

void help(char** args) {
  //open readme
  FILE* fp = fopen("readme", "r");
  if(fp == NULL) {
    printf("Can't find readme file.\n");
  }
  //get length of readme file so array can be large enough to hold it
  fseek(fp, 0, SEEK_END);
  int len = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char text[len];

  //if current stdout is the terminal, print some of readme file and then wait for user to hit enter to show more
  if(isatty(1)) {  
    //get height of window so we know how many lines to print at a time
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int rows = w.ws_row - 1;  
    
    int r = 1;
    //print first screen of readme file
    int lastpos = 0;
    int curpos;
    fgets(text, len, fp);
    do {
      printf("%s", text);
      //if line is longer than window is wide, we need to increment r again
      curpos = ftell(fp);
      int len = curpos - lastpos;
      while(len > w.ws_col) {
        r++;
        len -= w.ws_col;
      }
      r++;
      lastpos = curpos;
    }
    while(fgets(text, len, fp) != NULL && r < rows);
    //if we haven't reached EOF yet, wait for user to hit enter
   
    printf("%s", text);
    //now wait for user to hit enter to print next line or q to quit
    char* input;
    while(1) {
      input = readLine();
      if(strcmp(input, "q") == 0) {
      }
      else {
        if(fgets(text, len, fp) == NULL) {
          break;
        }
        else {
          //get rid of trailing newline from when user hit enter, then print line
          strtok(text, "\n");
          printf("%s", text);
        }
      }
    }
    printf("Reached end of help file. Press enter to return to the shell.\n");
    //wait for any key press & enter to exit the function
    getchar();
  }
  //STDOUT isn't the terminal, so just print the whole help file and don't wait for any user input
  else {
    while(fgets(text, len, fp) != NULL ) {
      printf("%s", text);
    }
  }
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
        help(args);
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
