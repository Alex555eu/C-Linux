#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#define MAX 255

using namespace std;

//struktura komunikatu
struct komunikat{
    long klient;
    char mtext[MAX-8];
};

    int i;
    int msize;
    struct komunikat kom;
    struct komunikat kom2;
    void get_text();
    int dp_zapis;
    int dp_odczyt;
    int errval;

int main(){

if (mkfifo("./fifo_serwer", 0606) == -1){
        perror("blad utworzenia fifo (serwer)\n");
        exit(EXIT_FAILURE);
 }


if((dp_odczyt = open("./fifo_serwer", O_RDONLY)) == -1){
        perror("blad otwarcia fifo serwera (serwer)\n");
        exit(EXIT_FAILURE);
}

sleep(10);

while(1){

if((errval = read(dp_odczyt, &kom, MAX+1)) == -1){
	perror("blad odczytu wiadomosci (serwer)\n");
	exit(EXIT_FAILURE);
}
else if(errval == 0){
	cout << "Serwer zakonczyl prace\n";
	break;
}
else {
cout << "Serwer odebrano: " << kom.mtext << " od procesu: " << kom.klient << endl;

    msize = strlen(kom.mtext);
    for(i=0;i<msize;i++){
            kom.mtext[i] = toupper(kom.mtext[i]);
    }

string file_path = "./fifo_" + to_string(kom.klient);

//sleep(1);
if((dp_zapis = open(file_path.c_str(), O_WRONLY)) == -1){
        perror("blad otwarcia fifo klienta (serwer)\n");
        exit(EXIT_FAILURE);
}

if(write(dp_zapis, &kom, sizeof(kom.mtext)+8+1) == -1){
	perror("blad zapisu wiadomosci (serwer)\n");
	exit(EXIT_FAILURE);
}

cout << "Serwer wyslano: " << kom.mtext << " --> " << kom.klient << endl;

if(close(dp_zapis) == -1){
	perror("close error (serwer)\n");
	exit(EXIT_FAILURE);
}

}
} // while out

if(close(dp_odczyt) == -1){
        perror("close() error (serwer)\n");
        exit(EXIT_FAILURE);
}

if(unlink("./fifo_serwer") == -1){
	perror("unlink error (serwer)\n");
	exit(EXIT_FAILURE);
}


exit(0);
}