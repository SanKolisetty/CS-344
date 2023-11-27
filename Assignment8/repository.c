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
    struct resource_ds *buf; 
    unsigned short *array;
};

int main()
{
    // ftok to generate unique key
    key_t key_shm = ftok("shmfile",65);

    // shmget returns an identifier in shmid
    int shmid = shmget(key_shm,900000,0666|IPC_CREAT);

    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid,(void*)0,0);

    // ftok to generate unique key
    key_t key_num = ftok("shmfile",64);

    // shmget returns an identifier in shmid
    int shmid_num = shmget(key_num,sizeof(int),0666|IPC_CREAT);

    // shmat to attach to shared memory
    int *num = (int*) shmat(shmid_num,(void*)0,0);
    num[0] = 0;

    // semaphore resource;           
    // semaphore rmutex;             
    // semaphore serviceQueue;

    int resource, rmutex, serviceQueue;
    key_t key, key_mutex, key_service;

    if ((key = ftok("writer.c", '2')) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((resource = semget(key, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg;
    sem_arg.val = 1;
    if (semctl(resource, 0, SETVAL, sem_arg) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    if ((key_mutex = ftok("writer.c", '3')) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((rmutex = semget(key_mutex, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg_mutex;
    sem_arg_mutex.val = 1;
    if (semctl(rmutex, 0, SETVAL, sem_arg_mutex) == -1) 
    {
        perror("semctl");
        exit(1);
    }
    
    if ((key_service = ftok("writer.c", '4')) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((serviceQueue = semget(key_service, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg_service;
    sem_arg_service.val = 1;
    if (semctl(serviceQueue, 0, SETVAL, sem_arg_service) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    FILE *fp = fopen("helloworld.txt", "r");

    printf("The shared memory is initialized with the following line:\n");
    printf("\n");
    char line[1024];
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
        strncpy(str, line, sizeof(line));
        str += strlen(str) + 1;
    }
    printf("\n");
    // char *str1 = (char*) shmat(shmid,(void*)0,0);

    // while(strlen(str1) != 0)
    // {
    //     char temp[1024];
    //     strncpy(temp, str1, strlen(str1) + 1);
    //     printf("%s", temp);
    //     str1 += strlen(str1) +1;
    // }

    sleep(300);

    //detach from shared memory
    shmdt(str);

    if (semctl(resource, 0, IPC_RMID, sem_arg) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    if (semctl(rmutex, 0, IPC_RMID, sem_arg_mutex) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    if (semctl(serviceQueue, 0, IPC_RMID, sem_arg_service) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    return 0;
}