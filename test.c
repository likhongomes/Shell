#include "utilities.h"

int main(int argc, char *argv[]){
    printf("Type something to test readLine, use spaces to check parser: ");
    char* userInput = readLine();
    printf("Output from readline: %s \n",userInput);
    char** inputArray = parse(userInput);
    int i = 0;
    printf("Output from the parser: \n");
    while(inputArray[i]!= '\0'){
        printf("%s \n", inputArray[i]);
        i++;
    }
    printf("Total %d words\n", i);
    
}