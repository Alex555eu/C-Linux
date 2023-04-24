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
#include <time.h>

using namespace std;
 
int sem_id;
int shm_id2;

void semafor_wait(int index);
void semafor_signal(int index);
char* message;

void sig_ctrl(int x);

int main(){

signal(SIGINT, sig_ctrl);
srand(time(NULL));

sem_id = semget(111, 3, 0200 | IPC_CREAT);
if (sem_id == -1) {
        perror( "semget error (pisarz)");
        exit(EXIT_FAILURE);
}

shm_id2 = shmget(222, 1, 0600 | IPC_CREAT);
if (shm_id2 == -1) {
        perror("shmget error (pisarz)");
        exit(EXIT_FAILURE);
}

message = (char*)shmat(shm_id2, 0, 0);
if (*message == -1) {
    perror("shmat erro (pisarz)");
    exit(EXIT_FAILURE);
}

char ch;

while(1){

    semafor_wait(1);
    ch = 65 + rand() % 25; //(char)(rand()%100);
    cout << "Pisarz: "<< ch <<endl;
    *message = ch;
sleep(1);
    semafor_signal(1);

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
            perror("Nie moglem zamknac semafora");
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
        perror("Blad otwarcia semafora");
        exit(EXIT_FAILURE);
    }
}

void sig_ctrl(int x){
    int shm;
    shm = shmdt(message);
    if (shm == -1) {
            perror("blad odlaczenia pamieci");
            exit(EXIT_FAILURE);
    }
    exit(0);
}
