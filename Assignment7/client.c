#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <math.h>
#include <wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <syslog.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf; 
    unsigned short *array;
};

void sem_wait(int semid)
{
    struct sembuf sb = {0, -1, 0};
    if (semop(semid, &sb, 1) == -1) 
    {
        exit(1);
    }   
}

void sem_signal(int semid)
{
    struct sembuf sb = {0, 1, 0};
    if (semop(semid, &sb, 1) == -1) 
    {
        exit(1);
    }   
}

int main()
{
    key_t key_shm;

    if ((key_shm = ftok("webserver.c", '1')) == -1) 
    {
        perror("ftok");
        exit(1);
    }

    int shmid = shmget(key_shm, 25*sizeof(int), 0666|IPC_CREAT);

    int semid, semid_full, semid_empty;
    key_t key, key_full, key_empty;
    
    // For mutex
    if ((key = ftok("webserver.c", '2')) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg;
    sem_arg.val = 1;
    if (semctl(semid, 0, SETVAL, sem_arg) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    // For full
    if ((key_full = ftok("webserver.c", '3')) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((semid_full = semget(key_full, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg_full;
    sem_arg_full.val = 0;
    if (semctl(semid_full, 0, SETVAL, sem_arg_full) == -1) 
    {
        perror("semctl");
        exit(1);
    }
    
    // For empty
    if ((key_empty = ftok("webserver.c", '4')) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((semid_empty = semget(key_empty, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg_empty;
    sem_arg_empty.val = 1;
    if (semctl(semid_empty, 0, SETVAL, sem_arg_empty) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    int *str = (int*) shmat(shmid,(void*)0,0);
    for(int i = 0; i < 100; i++)
    {
        sem_wait(semid_empty);
        sem_wait(semid);
        int tail = semctl(semid_empty,0,GETVAL)%25;

        // int x = rand();
        // str[tail] = x;
        // printf("Sent a HTTP request with number %d to the webserver\n.", x);

        str[tail] = i;
        printf("Sent a HTTP request with number %d to the webserver.\n", i);

        sem_signal(semid);
        sem_signal(semid_full);
    }

    sem_wait(semid_empty);
    sem_wait(semid);
    str[0] = -1;
    sem_signal(semid);
    sem_signal(semid_full);

    shmdt(str);
}