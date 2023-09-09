#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // forking for execle
    int x = fork();
    if(x == 0)
    {
        printf("execle is running!\n");
        // no arguments in execle
        execle("210101093-q1a", "./210101093-q1a", NULL, NULL);
        perror("error:");
    }
    else if(x == -1)
    {
        printf("Oops! Error while creating a child. Try again :(\n");
        exit(0);
    }
    else
    {
        // waiting for execle to be completed
        wait(NULL);

        // forking for execl
        x = fork();
        if(x == 0)
        {
            printf("execl is running!\n");
            // one argument in execl
            execl("210101093-q1a", "./210101093-q1a", "10", NULL);
            perror("error:");
        }
        else if(x == -1)
        {
            printf("Oops! Error while creating a child. Try again :(\n");
            exit(0);
        }
        else
        {
            // waiting for execl to be completed
            wait(NULL);

            // forking for execve
            x = fork();
            if(x == 0)
            {
                printf("execve is running!\n");
                // 10 arguments in execve
                char * args[] = {"./210101093-q1a", "1","2","3","4","5","6","7","8","9","10", NULL};
                execve("210101093-q1a", args, NULL);
                perror("error:");
            }
            else if(x == -1)
            {
                printf("Oops! Error while creating a child. Try again :(\n");
                exit(0);
            }
            else
            {
                // waiting for execve
                wait(NULL);
            }
        }
    }
}