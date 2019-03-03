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

int main(){
    pid_t pid;
    if((pid = fork())==0){
        printf("in child");


        execvp("emacs", NULL);
    } else if(pid > 0){
        printf("In parent \n");
        printf("%d", (int)pid);
        int status;
        waitpid(pid, &status,0);
    } else {
        printf("Process failed \n");
    }


}

