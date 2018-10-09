#include <iostream>

using namespace std;

int loop = 0;


void clear(){
  system("clear");
}


int main(int argc, char *argv[]){
  system("clear");
  string functions[] = {"cd","clr","dir","environ","echo","help","pause","quit"};
  while(loop != -1){

    string userInput;
    cout << "myShell$:";
    cin >> userInput;

    if(userInput == "clr"){
      printf("\033[H\033[2J");
    }


  }
}

void parse(){

}

void Quit(){
  loop = -1;
}
