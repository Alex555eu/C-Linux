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

#define w1 0
#define sp 1
#define sc 2
#define w2 3
#define w3 4

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

sem_id = semget(1111, 6, 0200 | IPC_CREAT);
if (sem_id == -1) {
        perror( "semget error (czytelnik)");
        exit(EXIT_FAILURE);
}

shm_id1 = shmget(2221, 1, 0600 | IPC_CREAT);
if (shm_id1 == -1) {
        perror("shmget error (czytelnik)");
        exit(EXIT_FAILURE);
}

shm_id2 = shmget(2222, 1, 0600 | IPC_CREAT);
if (shm_id2 == -1) {
        perror("shmget error (czytelnik)");
        exit(EXIT_FAILURE);
}

reader = (int*)shmat(shm_id1, 0, 0);
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

		semafor_wait(5);
		semafor_wait(w3);
		semafor_wait(sc);
		semafor_wait(w1);
//
			*reader=*reader+1;
			if(*reader==1) semafor_wait(sp);
			semafor_signal(w1);
			semafor_signal(sc);
			semafor_signal(w3);
			cout << "Czytam znak: " << *message << " czytelnikow w czytelni:"
			     << *reader << "/" << amount << endl;

			sleep(1);
			semafor_wait(w1);
			*reader=*reader-1;
			if(*reader==0) semafor_signal(sp);
//
			semafor_signal(w1);
			semafor_signal(5);
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
