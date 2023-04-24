#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <sys/resource.h>

long int amount_k;
long int amount_p;
long int amount_char;
int potok[2];
long int proc_val;
char *ptr;

int main(int argc, char* argv[]) {

printf("pid=%d\n",getpid());

if(argc != 4){
	printf("nie odpowiednia liczba argumentow!");
	exit(EXIT_FAILURE);
}

//
amount_k = strtol(argv[1],&ptr,10);
if(ptr == argv[1]){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}

if(amount_k >= INT_MAX || amount_k < 0){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}
//
amount_p = strtol(argv[2],&ptr,10);
if(ptr == argv[2]){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}

if(amount_p >= INT_MAX || amount_p < 0){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}
//
amount_char = strtol(argv[3],&ptr,10);
if(ptr == argv[3]){
printf("niepoprawny argument!");
exit(EXIT_FAILURE);
}

if(amount_char >= INT_MAX || amount_char < 0){
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

if((amount_p+amount_k+proc_val) > proc_limit){
	printf("przekroczono limit procesow!");
	exit(EXIT_FAILURE);
}

if(pipe(potok) == -1){
	perror("pipe error!");
	exit(EXIT_FAILURE);
}

int p = 0;
while(p < amount_p){
switch(fork()) {

        case -1:
                perror("fork error!");
                exit(EXIT_FAILURE);

        case 0:
//
                if(close(potok[0]) == -1){
                perror("close(potok) error!");
                exit(EXIT_FAILURE);
                }

                if(dup2(potok[1],1) == -1){
                perror("dup2(potok) error!");
                exit(EXIT_FAILURE);
                }

                if(execlp("./prod","prod",argv[3],NULL) == -1){
                perror("execl error!");
                exit(EXIT_FAILURE);
                }

        default:
                break;
}
p++;
}


int k=0;

while(k < amount_k){
switch(fork()) {

	case -1:
		perror("fork error!");
		exit(EXIT_FAILURE);

	case 0:
		if(close(potok[1]) == -1){
		perror("close(potok) error!");
		exit(EXIT_FAILURE);
		}

		if(dup2(potok[0],0) == -1){
		perror("dup2(potok) error!");
		exit(EXIT_FAILURE);
		}

		if(execlp("./kons","kons",NULL) == -1){
		perror("execl error!");
		exit(EXIT_FAILURE);
		}

	default:
		break;
}
k++;
}

for(int i = 0; i < amount_p; i++){
	wait(NULL);
}

if(close(potok[1]) == -1){
perror("close(potok) error!");
exit(EXIT_FAILURE);
}

if(close(potok[0]) == -1){
perror("close(potok) error!");
exit(EXIT_FAILURE);
}

for(int i = 0; i < amount_k; i++){
        wait(NULL);
}


exit(0);

}

