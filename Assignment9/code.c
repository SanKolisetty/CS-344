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

struct fork{
    char name[50];
    int used_by_pid[10];
};

struct fork* arr[5];
int semaphore[5];
int shmids_arr[5];

void init(int i)
{
    int x; key_t key;
    
    if ((key = ftok("code.c", i)) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((x = semget(key, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg;
    sem_arg.val = 1;
    if (semctl(x, 0, SETVAL, sem_arg) == -1) 
    {
        perror("semctl");
        exit(1);
    }
    semaphore[i] = x;

    key_t key_shm = ftok("shmfile",i);
    int shmid = shmget(key_shm,sizeof(struct fork),0666|IPC_CREAT);

    struct fork* ptr = (struct fork*) shmat(shmid,(void*)0,0);

    sprintf(ptr->name, "Fork %d", i);

    for(int i = 0; i < 10; i++)
    {
        ptr->used_by_pid[i] = -1;
    }
    arr[i] = ptr;
}

int main()
{
    for(int i = 0; i < 5; i++)
    {
        init(i);
    }

    key_t key_shm = ftok("shmfile",5);
    int shmid = shmget(key_shm,sizeof(int),0666|IPC_CREAT);
    int *count = (int*)shmat(shmid, (void*)0,0);
    count[0] = 0;

    int count_sem; key_t key;
    if ((key = ftok("code.c", 10)) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((count_sem = semget(key, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg;
    sem_arg.val = 1;
    if (semctl(count_sem, 0, SETVAL, sem_arg) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    int write_sem; 
    if ((key = ftok("code.c", 11)) == -1) 
    {
        perror("ftok");
        exit(1);
    }
    if ((write_sem = semget(key, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1);
    }
    union semun sem_arg1;
    sem_arg1.val = 1;
    if (semctl(write_sem, 0, SETVAL, sem_arg1) == -1) 
    {
        perror("semctl");
        exit(1);
    }

    int position;   
    pid_t pid;

    printf("ROUND 1\n");

    for(int i = 0; i < 5; i++)
    {
        pid = fork();
        if(pid == 0)
        {
            position = i;
            break;
        }
    }

    if(pid == 0)
    {
        int round = 0;

        while(round < 5)
        {
            sem_wait(write_sem);
            printf("\n");
            printf("Transaction %d is currently at the position %d.\n", getpid(), position);
            sem_signal(write_sem);

            if(position%2)
            {
                sem_wait(semaphore[position]);
                sem_wait(write_sem);
                printf("Transaction %d holding the semaphore %d.\n", getpid(), position);
                sem_signal(write_sem);
                for(int i = 0; i < 10; i++)
                {
                    if(arr[position]->used_by_pid[i] == -1)
                    {
                        arr[position]->used_by_pid[i] = getpid();

                        break;
                    }
                }

                sem_wait(semaphore[(position+1)%5]);
                sem_wait(write_sem);
                printf("Transaction %d holding the semaphore %d.\n", getpid(), (position+1)%5);
                sem_signal(write_sem);
                for(int i = 0; i < 10; i++)
                {
                    if(arr[(position+1)%5]->used_by_pid[i] == -1)
                    {
                        arr[(position+1)%5]->used_by_pid[i] = getpid();
                        break;
                    }
                }
            }
            else
            {
                sem_wait(semaphore[(position+1)%5]);
                sem_wait(write_sem);
                printf("Transaction %d holding the semaphore %d.\n", getpid(), (position+1)%5);
                sem_signal(write_sem);
                for(int i = 0; i < 10; i++)
                {
                    if(arr[(position+1)%5]->used_by_pid[i] == -1)
                    {
                        arr[(position+1)%5]->used_by_pid[i] = getpid();
                        break;
                    }
                }

                sem_wait(semaphore[position]);
                sem_wait(write_sem);
                printf("Transaction %d holding the semaphore %d.\n", getpid(), position);
                sem_signal(write_sem);
                for(int i = 0; i < 10; i++)
                {
                    if(arr[position]->used_by_pid[i] == -1)
                    {
                        arr[position]->used_by_pid[i] = getpid();
                        break;
                    }
                }
            }

            sem_wait(write_sem);
            printf("Transaction %d has written into the database tables and has completed it's round.\n", getpid());
            sem_signal(write_sem);
            sleep(10);

            if(position%2)
            {
                sem_signal(semaphore[position]);
                sem_signal(semaphore[(position+1)%5]);
            }
            else
            {
                sem_signal(semaphore[(position+1)%5]);
                sem_signal(semaphore[position]);
            }  

            sem_wait(count_sem);
            if(count[0] < 4)
            {
                count[0]++;
            }
            else
            {
                position = (position+1)%5;
                round++;
                printf("\n");
                if(round < 5)
                {
                	printf("ROUND %d\n", round+1);
                }
                count[0] = 0;
                sleep(5);
                sem_signal(count_sem);
                continue;
            }
            sem_signal(count_sem);

            position = (position+1)%5;
            round++;

            while(count[0] != 0);      
        }

        for(int i = 0; i < 5; i++)
        {
            shmdt(arr[i]);
        }
        shmdt(count);
    }
    else
    {
        wait(NULL);
        wait(NULL);
        wait(NULL);
        wait(NULL);
        wait(NULL);

        sem_wait(write_sem);
        printf("\n");
        printf("The database tables are given below:-\n");
        for(int i = 0; i < 5; i++)
        {
            printf("\n");
            printf("%s\n", arr[i]->name);

            for(int j = 0; j < 10; j++)
            {
                printf("%d ", arr[i]->used_by_pid[j]);
            }
            printf("\n");
        }
        sem_signal(write_sem);

        for(int i = 0; i < 5; i++)
        {
            shmdt(arr[i]);
            shmctl(shmids_arr[i], IPC_RMID, NULL);
            semctl(semaphore[i], 0, IPC_RMID);
        }
        shmdt(count);
        shmctl(shmid, IPC_RMID,NULL);
        semctl(count_sem, 0, IPC_RMID);
        semctl(write_sem, 0, IPC_RMID);
    }
}
