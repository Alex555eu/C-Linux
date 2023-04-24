//#include <stdio.h>
//#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <iostream>

#define MAX 255
#define SERWER 1
#define KLIENT 2


//struktura komunikatu
struct komunikat{

    long mtype;
	long klient;
    char mtext[MAX];

};

    int msize;
    int i, pid;
    int IDkolejki;
    struct komunikat kom;
    void queue_dlt();
    void sig_ctrlc(int s);

int main() {

   if((IDkolejki = msgget(777, IPC_CREAT | 0606)) == -1)
    {
        perror("Blad tworzenia kolejki komunikatow.\n");
        exit(EXIT_FAILURE);
    }
    printf("id=%d\n",IDkolejki);

    signal(SIGINT,sig_ctrlc);

//
/*
struct msqid_ds buf;
    if(msgctl(IDkolejki, IPC_STAT, &buf)==-1){
        perror("Blad podczas pobierania informacji o kolejce\n");
        queue_dlt();//exit(EXIT_FAILURE);
    }
    buf.msg_qbytes = (2*(MAX+1));
    if(msgctl(IDkolejki, IPC_SET, &buf)==-1){
        perror("Blad podczas ustawiania rozmiaru kolejki\n");
        queue_dlt();//exit(EXIT_FAILURE);
    }else{
        printf("rozmiar kolejki ustawiony na: %ld bitów\n", buf.msg_qbytes);
    }
*/
//

////////////////////////////////////////
while(1){

    printf("Serwer: Czekam na komunikat...\n");
    kom.mtype = SERWER;

    if((msgrcv(IDkolejki,(struct komunikat*)&kom, MAX+1, kom.mtype, 0)) == -1)
    {
        perror("Blad pobrania komunikatu z kolejki\n");
        queue_dlt();//exit(EXIT_FAILURE);
    }

    printf("Serwer: Odebrano: \"%s\" zaadresowane do %ld\n", kom.mtext, kom.mtype);

    msize = strlen(kom.mtext);
    for(i=0;i<msize;i++){
            kom.mtext[i] = toupper(kom.mtext[i]);
    }


    printf("serwer odebral komunikat od procesu: %ld\n", kom.klient);

    kom.mtype = kom.klient;

    printf("Serwer: Wysylanie... %s -> %ld \n",kom.mtext, kom.mtype);

    if((msgsnd(IDkolejki,(struct komunikat*)&kom, sizeof(long int)+strlen(kom.mtext)+1,0))==-1)
    {
            perror("Blad dodania komunikatu do kolejki (serwer).\n");
            queue_dlt();//exit(EXIT_FAILURE);
    }

}
    exit(0);
}

void queue_dlt() {
    if ( msgctl(IDkolejki,IPC_RMID,0)==-1 ) {
    perror("Blad usuniecia kolejki.\n");
    exit(EXIT_FAILURE);
    }
    else {
	printf("Kolejka zostala usunieta!");
	exit(0);
    }
}

void sig_ctrlc(int s) {
        queue_dlt();
}