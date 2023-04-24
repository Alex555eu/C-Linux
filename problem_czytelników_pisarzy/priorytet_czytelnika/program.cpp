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

long int amount_cz;
long int amount_pi;
long int total;
long int proc_val;

int shm_id1, shm_id2;
char* message;
int* reader;

char *ptr;

void sig_ctrl(int x);

int main(int argc, char* argv[]) {

signal(SIGINT, sig_ctrl); // ?

printf("pid=%d\n",getpid());

if(argc != 4){
	printf("nie odpowiednia liczba argumentow!");
	exit(EXIT_FAILURE);
}

//
amount_pi = strtol(argv[1],&ptr,10);
if(ptr == argv[1]){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}

if(amount_pi >= INT_MAX || amount_pi <= 0){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}
//
amount_cz = strtol(argv[2],&ptr,10);
if(ptr == argv[2]){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}

if(amount_cz >= INT_MAX || amount_cz <= 0){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}
//
total = strtol(argv[3],&ptr,10);
if(ptr == argv[3]){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}

if(total >= INT_MAX || total < 0){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}

//
struct rlimit rlim;
if(getrlimit(RLIMIT_NPROC,&rlim) == -1){
	perror("getrlimit error!");
	exit(EXIT_FAILURE);

}

int proc_limit = rlim.rlim_cur; // soft limit
//
  FILE *fp;
  char value[40];

  fp = popen("ps -ux | wc -l", "r");
  if (fp == NULL) {
      printf("nie wykonano polecenia ps!\n" );
      exit(EXIT_FAILURE);
  }

	fgets(value, sizeof(value), fp);
	char* ptr2;
	proc_val = strtol(value,&ptr2,10);
	proc_val = proc_val - 4;

  pclose(fp);
//
printf("limit procesow urzytkownika: %d\n", proc_limit);
printf("ilosc aktywnych procesow: %ld\n", proc_val);

if((amount_cz+amount_pi+proc_val) > proc_limit){
	printf("przekroczono limit procesow!");
	exit(EXIT_FAILURE);
}


int sem_id = semget(111, 3, 0200 | IPC_CREAT);
if (sem_id == -1) {
	perror( "semget error (program)");
	exit(EXIT_FAILURE);
}

int sem;

if((sem = semctl(sem_id, 0, SETVAL, 1)) == -1){
	perror("semctl error (program)");
	exit(EXIT_FAILURE);
}

if((sem = semctl(sem_id, 1, SETVAL, 1)) == -1){
        perror("semctl error (program)");
        exit(EXIT_FAILURE);
}

if((sem = semctl(sem_id, 2, SETVAL, 1)) == -1){
        perror("semctl error (program)");
        exit(EXIT_FAILURE);
}


shm_id1 = shmget(221, 1, 0606 | IPC_CREAT);
if(shm_id1 == -1){
	perror("shmget error (program)");
	exit(EXIT_FAILURE);
}

shm_id2 = shmget(222, 1, 0606 | IPC_CREAT);
if(shm_id2 == -1){
        perror("shmget error (program)");
        exit(EXIT_FAILURE);
}

reader = (int*)shmat(shm_id1, 0, 0);
if(*reader == -1){
	perror("shmat error (program)");
	exit(EXIT_FAILURE);
}

message = (char*)shmat(shm_id2, 0, 0);
if(*message == -1){
        perror("shmat error (program)");
        exit(EXIT_FAILURE);
}

for(int i = 0; i < amount_pi; i++){

	switch(fork()) {

	case -1: perror("fork error (program)");
		 exit(EXIT_FAILURE);

	case 0: if(execlp("./pi","pi", NULL) == -1){
			perror("execlp error (program)");
			exit(EXIT_FAILURE);
		}
	default:
		break;
	}

}

for(int i = 0; i < amount_cz; i++){

        switch(fork()) {

        case -1: perror("fork error (program)");
                 exit(EXIT_FAILURE);

        case 0: if(execlp("./cz","cz", argv[3],NULL) == -1){
                        perror("execlp error (program)");
                        exit(EXIT_FAILURE);
                }
        default:
                break;
        }

}


for(int i = 0; i < amount_pi+amount_cz; i++) wait(NULL);

sem = semctl(sem_id, 0, IPC_RMID);
if(sem == -1){
	perror("semctl error (program)");
	exit(EXIT_FAILURE);
}

int shm_rm;

if((shm_rm = shmctl(shm_id1, IPC_RMID, 0)) == -1){
	perror("shmctyl error (program)");
	exit(EXIT_FAILURE);
}

if((shm_rm = shmctl(shm_id2, IPC_RMID, 0)) == -1){
        perror("shmctyl error (program)");
        exit(EXIT_FAILURE);
}

exit(0);
}

void sig_ctrl(int x){

	system("pkill -f ./pi");
	system("pkill -f ./cz");
}
