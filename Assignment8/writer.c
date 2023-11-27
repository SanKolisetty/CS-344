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

void sem_wait(int resource)
{
    struct sembuf sb = {0, -1, 0};
    if (semop(resource, &sb, 1) == -1) 
    {
        exit(1);
    }   
}

void sem_signal(int resource)
{
    struct sembuf sb = {0, 1, 0};
    if (semop(resource, &sb, 1) == -1) 
    {
        exit(1);
    }   
}

int main()
{
    // ftok to generate unique key
    key_t key_shm = ftok("shmfile",65);

    // shmget returns an identifier in shmid
    int shmid = shmget(key_shm,900000,0666|IPC_CREAT);

    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid,(void*)0,0);

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

    printf("In line to be serviced\n");
    sem_wait(serviceQueue);
    printf("Waiting to get the access to shared memory\n");
    sem_wait(resource);
    printf("Got access to the shared memory\n");
    sem_signal(serviceQueue);
    printf("\n");
    printf("Writing.........\n");
    printf("\n");

    char check[1024];
    while(strlen(str) != 0)
    {
        char check[1024];
        strncpy(check, str, strlen(str) + 1);
        if(strcmp(check, "return 0;}\n") == 0)
        {
            char temp1[1024];
            sprintf(temp1, "printf(\"Hello written by pid: %d\");\n", getpid());
            printf("%s", temp1);
            strncpy(str, temp1, sizeof(temp1));
            str += strlen(temp1) + 1;
            char temp[] = "return 0;}\n";
            printf("%s", temp);
            strncpy(str, temp, sizeof(temp));
            break;
        }
        printf("%s", check);
        str += strlen(str) +1;
    }
    printf("\n");


    sleep(10);

    printf("Done writing......\n");
    printf("\n");

    sem_signal(resource);
    printf("Resource signalled\n");
    printf("\n");

    shmdt(str);

}