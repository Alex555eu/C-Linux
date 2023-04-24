#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>

using namespace std;

int main(int argc, char* argv[]){

char* ptr;
long int amount = strtol(argv[1],&ptr,10);
char ch;
int i=0;

srand(time(NULL));

string name = "we_" + to_string(getpid()) + ".txt";

fstream mfile;

mfile.open(name, ios::out);

if(!mfile){
perror("nie mozna utworzyc pliku we_");
}
else {

while(i < amount){

 	ch = (rand() % 10) + '0';
	mfile << ch;
	write(1,&ch,sizeof(char));
	i++;
}

}
mfile.close();



exit(0);

}

