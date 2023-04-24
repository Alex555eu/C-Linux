#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <sys/resource.h>
 
using namespace std;

int sem_id;
int shm_id1, shm_id2;

void semafor_wait(int index);
void semafor_signal(int index);
char* message;
int* reader;
int amount;
char* ptr;

void sig_ctrl(int x);

int main(int argc, char* argv[]){

amount = strtol(argv[1], &ptr,10);
if(ptr == argv[1]){
    printf("niepoprawny argument!");
    exit(EXIT_FAILURE);
}
if (amount > INT_MAX || amount <= 0){
    printf("niepoprawny argument!");
    exit(EXIT_FAILURE);
}

sem_id = semget(111, 3, 0200 | IPC_CREAT);
if (sem_id == -1) {
        perror( "semget error (czytelnik)");
        exit(EXIT_FAILURE);
}

shm_id1 = shmget(221, 1, 0600 | IPC_CREAT);
if (shm_id1 == -1) {
        perror("shmget error (czytelnik)");
        exit(EXIT_FAILURE);
}

shm_id2 = shmget(222, 1, 0600 | IPC_CREAT);
if (shm_id2 == -1) {
        perror("shmget error (czytelnik)");
        exit(EXIT_FAILURE);
}

reader = (int*)shmat(shm_id1, 0, 0)
if (*reader == -1) {
    perror("shmat erro (czytelnik)");
    exit(EXIT_FAILURE);
}

message = (char*)shmat(shm_id2, 0, 0);
if (*message == -1) {
    perror("shmat erro (czytelnik)");
    exit(EXIT_FAILURE);
}

while(1){

semafor_wait(0);//

if(*reader < amount ){

	*reader +=1;

	if (*reader == 1 ) semafor_wait(1);

	semafor_signal(0);//

	cout<<"Czytelnik przeczytal: "<< *message<< endl;

sleep(1);

	semafor_wait(0);//

	*reader -=1;


	if (*reader == 0 ) semafor_signal(1);

}

semafor_signal(0);//


}


    exit(0);
}

void semafor_wait(int index) {
    int sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num = index;
    bufor_sem.sem_op = -1;
    bufor_sem.sem_flg = 0;

    sem = semop(sem_id, &bufor_sem, 1);
    if (sem == -1) {
        if (errno == EINTR) {
            semafor_wait(index);
        }
        else {
            perror("semafor_wait error (pisarz)");
            exit(EXIT_FAILURE);
        }
    }
}


void semafor_signal(int index) {
    int sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num = index;
    bufor_sem.sem_op = 1;
    bufor_sem.sem_flg = 0;
    sem = semop(sem_id, &bufor_sem, 1);
    if (sem == -1){
        perror("semafor_signal error (pisarz)");
        exit(EXIT_FAILURE);
    }
}

void sig_ctrl(int x){
    int shm;
    shm = shmdt(message);
    if (shm == -1) {
            perror("sig_ctrl error (pisarz)");
            exit(EXIT_FAILURE);
    }

    shm = shmdt(reader);
    if (shm == -1) {
            perror("sig_ctrl error (pisarz)");
            exit(EXIT_FAILURE);
    }

    exit(0);
}
