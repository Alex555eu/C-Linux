#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <errno.h>

#define MAX 255 //maksymalny rozmiar wiadomosci
#define SERWER 1
#define KLIENT 2

using namespace std;

//struktura komunikatu
struct komunikat{

    long mtype;
    long klient;
    char mtext[MAX-8];

};

    int i;
    int IDkolejki;
    struct komunikat kom;
    struct komunikat kom2;
    struct msqid_ds msg;
    string text="";
    int amount=2;
    int static_amount = amount;
    int sentmsg=0;

    void get_text();

void*  thread1(void* _wat) {

        komunikat* kom = (komunikat*)_wat;
int x = amount;
while(x > 0){
	while(1) {
        cout << "Podaj tresc komunikatu: ";

        get_text();

        if((text.length()) > (MAX-8)) {
                cout << "Wiadomosc za dluga!\n";
                text="";
                continue;
        }
        else {
		//memset(kom->mtext, 0, MAX-8);
		strcpy(kom->mtext, text.c_str());
	     }
	text="";
//
       msgctl(IDkolejki,IPC_STAT,&msg);
        if((strlen(kom->mtext)+msg.msg_cbytes+sizeof(long int)+1) > (msg.msg_qbytes - MAX - 1)) {//bez +1 oraz >= zamiast >
                cout << "Brak miejsca w kolejce!\n";
                continue;
        }
        else break;
//
 }

        kom->mtype = SERWER;

                cout << "Klient: Wysylanie... " << kom->mtext << "-> " << kom->mtype << endl;

                if(msgsnd(IDkolejki, kom, sizeof(long int)+strlen(kom->mtext)+1,IPC_NOWAIT) == -1)
                {
                        perror("Blad dodania komunikatu do kolejki (klient)\n");
			amount = sentmsg - (static_amount - amount);
			if(sentmsg == 0 || amount == 0) exit(EXIT_FAILURE);
			pthread_exit(0);

                }
x--;
sentmsg++;
}
        pthread_exit(0);
}

void* thread2(void* _wat) {
	komunikat* kom = (komunikat*) _wat;

	while (amount > 0){
        if(msgrcv(IDkolejki, (struct komunikat *) kom, MAX+1, kom->klient,0) == -1)
        {
                perror("Blad odbioru komunikatu z kolejki (klient)\n");
                exit(EXIT_FAILURE);
        }

        cout << "Klient: Odebrano: " << kom->mtext << " zaadresowane do " << kom->mtype << endl;
	amount--;
	}
	pthread_exit(0);
}

int main() {
        int stworz_watek1;
        int dolacz_watek1;
        int odlacz_watek1;

        int stworz_watek2;
        int dolacz_watek2;
        int odlacz_watek2;

        pthread_t id_watku1;
        pthread_t id_watku2;

	kom.klient = getpid();
	kom2.klient = getpid();

        cout << "PID klienta= " << kom.klient << endl;
        if((IDkolejki = msgget(777, IPC_CREAT | 0606)) == -1)
        {
                perror("Blad tworzenia kolejki komunikatow.\n");
                exit(EXIT_FAILURE);
        }
        cout << "id=" << IDkolejki << endl;

        if((stworz_watek1 = pthread_create(&id_watku1, NULL,  &thread1, &kom)) == -1) {
		perror("nie moglem stworzyc watku 1.");
		exit(EXIT_FAILURE);
	}

	if((stworz_watek2 = pthread_create(&id_watku2, NULL, &thread2, &kom2)) == -1) {
		perror("nie moglem stworzyc watku 2.");
		exit(EXIT_FAILURE);
	}

        if((dolacz_watek1 = pthread_join(id_watku1, NULL)) == -1 ) {
		perror("nie moglem przylaczyc watku 1.");
		exit(EXIT_FAILURE);
	}

        if((dolacz_watek2 = pthread_join(id_watku2, NULL)) == -1) {
		perror("nie moglem przylaczyc watku 2.");
		exit(EXIT_FAILURE);
	}

        if((odlacz_watek2 = pthread_detach(id_watku2)) == -1 ) {
		perror("nie moglem odlaczyc watku 2.");
		exit(EXIT_FAILURE);
	}

	if((odlacz_watek1 = pthread_detach(id_watku1)) == -1 ) {
		perror("nie moglem odlaczyc watku 1.");
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

