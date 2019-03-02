#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    pid_t pid;
    if((pid = fork())==0){
        printf("in child");


        execvp("emacs", NULL);
    } else if(pid > 0){
        printf("In parent \n");
        printf("%d", (int)pid);
    } else {
        printf("Process failed \n");
    }


}

