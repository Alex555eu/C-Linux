#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

int semafor;
int key_sem;
static void utworz_nowy_semafor(int key_sem, int amount_sem);
static void ustaw_semafor(int semnum, int arg);
static void semafor_p(int sem_num1);
static void semafor_v(int sem_num1);
static void usun_semafor(void);
static void get_key();

int pamiec;
int odlaczenie1;
int odlaczenie2;
int key_shm;
char *adres;
void shmget();
void shmat();
void odlacz_pamiec();
static void get_key_shm();


int main()
{
    get_key_shm();
    shmget();
    shmat();

    get_key();
    utworz_nowy_semafor(key_sem, 3);
    ustaw_semafor(0,1);
    ustaw_semafor(1,0);
    ustaw_semafor(2,0);

    semafor_v(2);
    char ch;
    FILE *file;

    file = fopen("exit.txt", "w");

    if(file == NULL) {
        cout << "Nie mozna otworzyc pliku (konsument)";
        exit(-1);
    }

    while(ch != EOF) {

        semafor_p(1);

        ch = *adres;

        if(ch != EOF) {
           fputc(ch, file);
           semafor_v(0);
        }
    }

    fclose(file);

    odlacz_pamiec();

    usun_semafor();


    exit(0);
}


static void utworz_nowy_semafor(int key_sem, int amount_sem)
  {
    semafor=semget(key_sem,amount_sem,0200|IPC_CREAT);
    if (semafor==-1)
      {
        cout << "Nie moglem utworzyc nowego semafora." << endl;
        exit(EXIT_FAILURE);
      }
    else
      {
        cout << "Semafor zostal utworzony: " << semafor << endl;
      }
  }


static void ustaw_semafor(int semnum, int arg)
  {
    int ustaw_sem;
    ustaw_sem=semctl(semafor,semnum,SETVAL,arg);
    if (ustaw_sem==-1)
      {
        cout << "Nie mozna ustawic semafora." << endl;
        exit(EXIT_FAILURE);
      }
    else
      {
        cout << "Semafor zostal ustawiony." << endl;
      }
  }


static void semafor_p(int sem_num1)
  {
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num=sem_num1;
    bufor_sem.sem_op=-1;
    bufor_sem.sem_flg=0;

    zmien_sem=semop(semafor,&bufor_sem,1);

    if (zmien_sem==-1)
      {
        if(errno == EINTR){
        semafor_p(sem_num1);
        }
        else
        {
        cout << "Nie moglem zamknac semafora: " << sem_num1 << endl;
        exit(EXIT_FAILURE);
        }
      }
  }

static void semafor_v(int sem_num1)
  {
    int zmien_sem;
    struct sembuf bufor_sem;
    bufor_sem.sem_num=sem_num1;
    bufor_sem.sem_op=1;
    bufor_sem.sem_flg=0;

    zmien_sem=semop(semafor,&bufor_sem,1);

    if (zmien_sem==-1)
      {
        cout << "Nie moglem otworzyc semafora." << endl;
        exit(EXIT_FAILURE);
      }
  }


static void usun_semafor(void)
  {
    int sem;
    sem=semctl(semafor,0,IPC_RMID);
    if (sem==-1)
      {
        cout << "Nie mozna usunac semafora." << endl;
        exit(EXIT_FAILURE);
      }
    else
      {
        cout << "Semafor zostal usuniety: " << sem << endl;
      }
  }


static void get_key()
{
    key_sem = ftok(".",'A');

    if(key_sem == -1)
    {
        cout << "Blad tworzenia klucza (konsumer)" << endl;;
        exit(-1);
    }
}


static void get_key_shm(void)
{
    key_shm = ftok(".",'M');

    if(key_shm == -1)
    {
        cout << "Blad tworzenia klucza pamieci (konsumer)" << endl;
        exit(-1);
    }
}


void shmget()
  {
        pamiec=shmget(key_shm,1,0600|IPC_CREAT);
    if (pamiec==-1)
      {
        cout << "Problemy z utworzeniem pamieci dzielonej." << endl;
        exit(EXIT_FAILURE);
      }
    else cout << "Pamiec dzielona zostala utworzona: " << pamiec << endl;
  }


void shmat()
  {
    adres= (char*) shmat(pamiec,0,0);
    if (*adres==-1)
      {
        cout << "Problem z przydzieleniem adresu." << endl;
        exit(EXIT_FAILURE);
      }
    else cout << "Przestrzen adresowa zostala przyznana: " << adres << endl;
  }


void odlacz_pamiec()
  {
    odlaczenie1=shmctl(pamiec,IPC_RMID,0);
//sleep(5);
    odlaczenie2=shmdt(adres);
    if (odlaczenie1==-1 || odlaczenie2==-1)
      {
        cout << "Problemy z odlaczeniem pamieci dzielonej." << endl;
        exit(EXIT_FAILURE);
      }
    else cout << "Pamiec dzielona zostala odlaczona." << endl;
  }