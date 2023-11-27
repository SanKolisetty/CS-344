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

int main()
{
    // Getting input
    char input[100];
    int i = 0;
    while(1)
    {
        char c = getchar();
        if(c == '\n')
        {
            input[i] = '\0';
            break;
        }
        input[i] = c;
        i++;
    }
    if(strcmp(input, "quit") == 0)
    {
        return 0;
    }

    // Parsing the string
    char first[100], second[100], third[100];
    i = 0;
    while(input[i+1] != '|')
    {
        first[i] = input[i];
        i++;
    }
    first[i] = '\0';
    i += 3;
    int j = 0;
    while(input[i+1] != '|')
    {
        second[j] = input[i];
        i++; j++;
    }
    second[j] = '\0';
    j = 0;
    i += 3;
    while(input[i] != '\0')
    {
        third[j] = input[i];
        i++; j++;
    }
    third[j] = '\0';

    // printf("%s\n", first);
    // printf("%s\n", second);
    // printf("%s\n", third);
    
    // Creating pipes
    int fd1[2];
    int fd2[2];
    if(pipe(fd1) == -1 || pipe(fd2))
    {
        printf("There was some error while creating pipes.\n");
        return 0;
    }

    // First fork
    pid_t p = fork();
    if(p <  0)
    {
        printf("There was an error while forking a child.\n");
        exit(0);
    }
    if(p == 0)
    {
        // Executing first command
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);

        execlp("/bin/sh", "/bin/sh", "-c", first, NULL);
    }
    else
    {
        // Second fork
        p = fork();
        if(p < 0)
        {
            printf("There was an error while forking a child.\n");
            exit(0);
        }
        if(p == 0)
        {
            // Executing second command
            dup2(fd1[0], STDIN_FILENO);
            dup2(fd2[1], STDOUT_FILENO);
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);

            execlp("/bin/sh", "/bin/sh", "-c", second, NULL);     
        }
        else
        {
            // Third fork
            p = fork();
            if(p < 0)
            {
                printf("There was an error while forking a child.\n");
                exit(0);
            }
            if(p == 0)
            {
                // Executing third command
                dup2(fd2[0], STDIN_FILENO);
                close(fd1[0]);
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);

                execlp("/bin/sh", "/bin/sh", "-c", third, NULL);          
            }
            else
            {
                // Closing all the file descriptors
                close(fd1[0]);
                close(fd1[1]);
                close(fd2[0]);
                close(fd2[1]);

                // Waiting for all the children
                wait(NULL);
                wait(NULL);
                wait(NULL);
            }
        }
    }
}