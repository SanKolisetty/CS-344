#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    // forking for execl
    int x = fork();
    if(x == 0)
    {
        unsetenv("n");
        printf("execl is running!\n");
        // no arguments in execl
        execl("210101093-q1b", "./210101093-q1b", NULL);
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

        // forking for execl
        x = fork();
        if(x == 0)
        {
            printf("execle is running!\n");
            // one argument in execle
            char * env[] = {"n=23", NULL};
            execle("210101093-q1b", "./210101093-q1b", NULL, env);
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
                char * args[] = {"./210101093-q1b", NULL};
                char * env[] = {"n=2 4 6 8 10 12 14 16 18 20", NULL};
                execve("210101093-q1b", args, env);
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