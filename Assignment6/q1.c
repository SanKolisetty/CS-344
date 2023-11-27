// To compile
// gcc q1.c
// To run
// ./a.out
// Please run the files concurrently

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>

struct msg
{
    long int msg_type;
    int row_number;
    int column_number;
};

int main()
{
    struct msg values;
    int msqid;
    key_t key;

    if ((key = ftok("q1.c", 'a')) == -1)
    {
        perror("ftok");
        exit(1);
    }

    if ((msqid = msgget(key, 0644 | IPC_CREAT)) == -1)
    {
        perror("msgget");
        exit(1);
    }

    values.msg_type = 1;

    for(int i = 1; i <= 256; i++)
    {
        for(int j = 1; j <= 256; j++)
        {
            values.row_number = i; 
            values.column_number = j;
            if(msgsnd(msqid, (struct msgbuf *)&values, sizeof(values), 0) == -1)
            {
                perror("-1");
            }
            printf("%d %d is written in the queue.\n", values.row_number, values.column_number);
        }
    }
    printf("I'm going to sleep for 25 seconds before closing the message queue so that the other process reads all the values.\n");
    sleep(25);
    if (msgctl(msqid, IPC_RMID, NULL) == -1) 
    {
        perror("msgctl");
        exit(1);
    }
}
