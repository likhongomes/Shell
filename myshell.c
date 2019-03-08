#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

/*
Likhon D. Gomes
CIS 3207
Lab 2 - Shell
*/

//this is where all the execution happens
void execute(char **args);
int makePipe(char** args1, char** args2);

const int bufferSize = 1024;
int loop = 0;

//this is the readline functoin
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

//a simple quit function that sets the loop variable to -1
void quit(){
  loop = -1;
}

//This is the cd function
void cd(char** args){
  if(chdir(args[1])!=0){
    printf("directory not found");
  }
}

//function for clearing screen without invoking any system call
void clr(){
  printf("\033[H\033[2J]");
}

//This is the dir function.
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
  // printing out all the files from the directory.
  while((contents = readdir(dir)) != NULL){
    printf("%s ", contents->d_name);
  }
  printf("\n");
  closedir(dir);
}

//Function to show environment
void environ(){
  int numOfVars = 7;
  char* info[] = {"USER","HOME","PATH","SHELL","OSTYPE","PWD","GROUP"};

  int i;
  //getting those environment variables
  for(i = 0; i < numOfVars; i++){
    const char* var = getenv(info[i]);
    if(var != NULL){
      printf("%s: %s\n",info[i], var);
    } else {
      printf("Error: Could not fetch the environment variables.");
    }
  }
}

//function for echo
void echo(char** args){
  int i = 1;
  while(args[i] != NULL){
    printf("%s ",args[i]);
    args[i] = NULL;
    i++;
  }
  printf("\n");
}

//function to print out help
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

int makePipe(char** args1, char** args2) {
  pid_t pid1, pid2;
  int fd[2];
  //create pipe
  pipe(fd);

  //fork to exec first program
  pid1 = fork();
  if(pid1 < 0) {
    printf("Error forking.\n");
  }
  else if(pid1 == 0) {
    //send stdout to write end of pipe
    dup2(fd[1], STDOUT_FILENO);
    //close read end of pipe
    close(fd[0]);
    execute(args1);
    exit(0);
  }
  else {
    //returned to parent process
    //fork to exe second program
    pid2 = fork();
    if(pid2 < 0) {
      printf("Error forking.\n");
    }
    else if(pid2 == 0) {
      //send stdin to read end of pipe
      dup2(fd[0], STDIN_FILENO);
      //close write end of pipe
      close(fd[1]);
      execute(args2);
      exit(0);
    }
    else {
      //we are in parent, wait for both child processes to finish 
      //close the pipe in parent process
      close(fd[0]);
      close(fd[1]);
      //wait for both children to finish
      waitpid(pid1, NULL, 0);
      waitpid(pid2, NULL, 0);
    }
  }
  return 1;
}














void execute(char **args){

  int runBg = 0; // running in the background
  int builtIn = 0;
  int redirectIn = 0;
  int redirectOut = 0;
  int i, j;
  int stdinDup, stdoutDup, infile, outfile;

  if(args[0] == NULL){
    //checking if the args list is empty
  }else{

//checks through all the symbols possible
    i = 0;
  while(args[i] != NULL) {
   if(strcmp(args[i], "&") == 0) {
      runBg = 1;
    } 
   else if(strcmp(args[i], "<") == 0) {
      infile = open(args[i+1], O_RDONLY);
      args[i] = NULL;
      redirectIn = 1;
    }
    else if(strcmp(args[i], ">") == 0) {
      outfile = open(args[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR | O_CLOEXEC);
      args[i] = NULL;
      redirectOut = 1;
    } 
    else if(strcmp(args[i], ">>") == 0) {
      outfile = open(args[i+1], O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR | O_CLOEXEC);
      args[i] = NULL;
      redirectOut = 1;
    }
    else if(strcmp(args[i], "|") == 0) {
      //set | argument to null, then call makePipe passing args before pipe and args after pipe
      makePipe(&args[0],&args[i+i]);
      args[i] = NULL;

    }
    i++;
  }

    /*
    pid_t pid;
    if((pid = fork())==0){
        printf("in child");
        execvp(args[i], args);
    } else if(pid > 0){
        printf("In parent \n");
        int status = 0;
        waitpid(pid, &status, 0);
        printf("%d", (int)pid);
    } else {
        printf("Process failed \n");
    }*/


    
    //checking for internal functions
      if(strcmp(args[0],"exit")==0){
        //save current i/o values so they can be restored later
        stdinDup = dup(STDIN_FILENO);
        stdoutDup = dup(STDOUT_FILENO);
        if(redirectIn == 1) {
        dup2(infile, STDIN_FILENO);
        }
        if(redirectOut == 1) {
         dup2(outfile, STDOUT_FILENO);
        }
        quit();
        dup2(stdoutDup, STDOUT_FILENO);
        dup2(stdinDup, STDIN_FILENO);
      } else if (strcmp(args[0],"clear")==0){
        //save current i/o values so they can be restored later
        stdinDup = dup(STDIN_FILENO);
        stdoutDup = dup(STDOUT_FILENO);
        if(redirectIn == 1) {
        dup2(infile, STDIN_FILENO);
        }
        if(redirectOut == 1) {
          dup2(outfile, STDOUT_FILENO);
        }
        clr();
        dup2(stdoutDup, STDOUT_FILENO);
        dup2(stdinDup, STDIN_FILENO);
      } else if (strcmp(args[0],"echo")==0){
        //save current i/o values so they can be restored later
        stdinDup = dup(STDIN_FILENO);
        stdoutDup = dup(STDOUT_FILENO);
        if(redirectIn == 1) {
          dup2(infile, STDIN_FILENO);
        }
        if(redirectOut == 1) {
          dup2(outfile, STDOUT_FILENO);
        }
        echo(args);
        dup2(stdoutDup, STDOUT_FILENO);
        dup2(stdinDup, STDIN_FILENO);
      }else if (strcmp(args[0],"cd")==0){
        //save current i/o values so they can be restored later
        stdinDup = dup(STDIN_FILENO);
        stdoutDup = dup(STDOUT_FILENO);
        if(redirectIn == 1) {
          dup2(infile, STDIN_FILENO);
        }
        if(redirectOut == 1) {
          dup2(outfile, STDOUT_FILENO);
        }
        cd(args);
        dup2(stdoutDup, STDOUT_FILENO);
        dup2(stdinDup, STDIN_FILENO);
      } else if (strcmp(args[0],"dir")==0){
        //save current i/o values so they can be restored later
        stdinDup = dup(STDIN_FILENO);
        stdoutDup = dup(STDOUT_FILENO);
        if(redirectIn == 1) {
          dup2(infile, STDIN_FILENO);
        }
        if(redirectOut == 1) {
          dup2(outfile, STDOUT_FILENO);
        }
        dir(args);
        dup2(stdoutDup, STDOUT_FILENO);
        dup2(stdinDup, STDIN_FILENO);
      } else if (strcmp(args[0],"environ")==0){
        //save current i/o values so they can be restored later
        stdinDup = dup(STDIN_FILENO);
        stdoutDup = dup(STDOUT_FILENO);
        if(redirectIn == 1) {
          dup2(infile, STDIN_FILENO);
        }
        if(redirectOut == 1) {
          dup2(outfile, STDOUT_FILENO);
        }
        environ();
        dup2(stdoutDup, STDOUT_FILENO);
        dup2(stdinDup, STDIN_FILENO);
      } else if (strcmp(args[0],"help")==0){
        //save current i/o values so they can be restored later
        stdinDup = dup(STDIN_FILENO);
        stdoutDup = dup(STDOUT_FILENO);
        if(redirectIn == 1) {
          dup2(infile, STDIN_FILENO);
        }
        if(redirectOut == 1) {
          dup2(outfile, STDOUT_FILENO);
        }
        help(args);
        dup2(stdoutDup, STDOUT_FILENO);
        dup2(stdinDup, STDIN_FILENO);
      } else {





      //not a builtin function
      pid_t pid = fork();

        if(pid == 0){
          //printf("in child");
          execvp(args[0],args);       
          fprintf(stderr, "myshell>: %s: command not found\n",args[0]); /*If execvp failes*/
          exit(1);
        } else if(pid>0){
          //printf("in parent");
          if(runBg==0){
            waitpid(pid,NULL,0);
          } else {
            printf("Background process %d",(int)pid);
          }
          
          


        } else {
          printf("fork failed");
        }


        /*
        if(pid < 0){
          printf("fork failed");
        } else if ( pid == 0){
          execvp(args[0], args);
          printf("command or executable file not recognized\n");
        } else {
          if(runBg == 0){
            waitpid(pid, NULL,0);
          }
      }*/
      }
    }
  }

void singleCommandMode(){
  printf("\033[H\033[2J]");
  while (loop != -1) {
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("%s/myshell>", cwd);
    char* userInput = readLine();
    char** inputArray = parse(userInput);
    //printf("%s",inputArray[0]);
    execute(inputArray);
  }
}

void batchExecution(char filename[100]){
  printf("Executing Batch File\n");

  FILE *file;
  char line[200];
  char **args;
  file = fopen(filename, "r");
  if (file == NULL) {
    printf("Unable to open file");
  } else {
    while(fgets(line, sizeof(line), file)!= NULL){
      printf("\n%s",line);
      args=parse(line);
      execute(args);
      ///////////
    }
  }
  free(args);
  fclose(file);
}

//This is the main function
int main(int argc, char *argv[]){

  if(argc == 1){ //single command execution
    singleCommandMode();
  } else if (argc == 2){ //batch file execution
    batchExecution(argv[1]);
  } else {
    printf("Invalid Number of Arguments\n");
  }


  
}
