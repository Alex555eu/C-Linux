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

using namespace std;

int main(int argc, char* argv[]){

char bufor;
int zmienna;
printf("pid kons=%d\n",getpid());

string name = "wy_" + to_string(getpid()) + ".txt";

fstream mfile;

mfile.open(name, ios::out);

if(!mfile){
	printf("nie mozna utworzyc pliku wy_");
}

else{

while((zmienna = read(0,&bufor,sizeof(bufor))) > 0){
	mfile << bufor;

}
mfile.close();
}



printf("konsument koniec!!\n");

exit(0);

}

