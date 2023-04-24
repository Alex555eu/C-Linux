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

    struct komunikat kom;
    struct komunikat kom2;
    string text="";
    void get_text();
    void input();
    int dp_zapis;
    int dp_odczyt;

int main(){

string file_path = "./fifo_" + to_string(getpid());

if((dp_zapis = open("./fifo_serwer", O_WRONLY)) == -1){
        perror("blad otwarcia fifo serwera (klient)\n");
        exit(EXIT_FAILURE);
}


if (mkfifo(file_path.c_str(), 0606) == -1){
        perror("blad utworzenia fifo (klient)\n");
        exit(EXIT_FAILURE);
 }

int i = 2;
while(i>0){

	input(); // pobiera tekst do wyslania


        kom.klient = getpid();


cout << "Klient (" << getpid() << "): Wysylanie... " << kom.mtext << "-> serwer" << endl;

if(write(dp_zapis, &kom, sizeof(kom.mtext)+8+1) == -1){
	perror("blad zapisu do serwera (klient)\n");
	exit(EXIT_FAILURE);
}

if((dp_odczyt = open(file_path.c_str(), O_RDONLY)) == -1){
        perror("blad otwarcia fifo klienta (klient)\n");
        exit(EXIT_FAILURE);
}

int r_val;

while((r_val = read(dp_odczyt, &kom2, MAX+1)) == 0){
//	sleep(1);
}

if(r_val == -1){
        perror("blad odczytu wiadomosci (klient)\n");
        exit(EXIT_FAILURE);

}

cout << "Klient odebrano: " << kom2.mtext << " zaadresowane do: " << kom2.klient << endl;

if(close(dp_odczyt) == -1){
        perror("close error (klient)\n");
        exit(EXIT_FAILURE);
}


i--;
} // while out

if(close(dp_zapis) == -1){
        perror("close error (klient)\n");
        exit(EXIT_FAILURE);
}

if(unlink(file_path.c_str()) == -1){
        perror("unlink error (klient)\n");
        exit(EXIT_FAILURE);
}



exit(0);
}

void get_text(){
        while(1){
        string temp;
        //cin.ignore();
        getline(cin, temp);
        if(temp == "") break;
        if(!text.empty()) text = text + "\n";
        text = text + temp;
        }
}

void input(){
        while(1) {
        cout << "Podaj tresc komunikatu: ";

        get_text();

        if((text.length()) > (MAX-8)) {
                cout << "Wiadomosc za dluga!\n";
                text="";
                continue;
        }
        else {
                strcpy(kom.mtext, text.c_str());
             }
        text="";
        break;
 }

}

