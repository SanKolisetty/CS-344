#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <syslog.h>
#include <sys/resource.h>
#include <syslog.h>
#include <signal.h>
#include <sys/wait.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
    // ftok to generate unique key
    key_t key = ftok("shmfile",65);

    // shmget returns an identifier in shmid
    int shmid = shmget(key,900000,0666|IPC_CREAT);

    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid,(void*)0,0);

    FILE *fp = fopen("week05-ML2-input.txt", "r");

    char line[1024];
    while(fgets(line, sizeof(line), fp) != NULL)
    {
        printf("%s", line);
        strncpy(str, line, sizeof(line));
        str += strlen(str) + 1;
    }
    
    //detach from shared memory
    shmdt(str);

    return 0;
}