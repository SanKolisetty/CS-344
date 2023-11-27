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
    while(1)
    {
        printf("To get the sequence, type './compute sequence 1 | ./compute sequence 2'. To exit, type 'quit'\n");

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
        if(strcmp(input, "./compute sequence 1 | ./compute sequence 2") != 0 && strcmp(input, "quit") != 0)
        {
            printf("Please enter an appropriate string.\n");
            continue;   
        }
        if(strcmp(input, "quit") == 0)
        {
            return 0;
        }

        // Parsing the string
        char left[100], right[100];
        i = 0;
        while(input[i+1] != '|')
        {
            left[i] = input[i];
            i++;
        }
        left[i] = '\0';
        i += 3;
        int j = 0;
        while(input[i] != '\0')
        {
            right[j] = input[i];
            i++; j++;
        }
        right[j] = '\0';

        // Creating pipe
        int fd[2];
        if(pipe(fd) == -1)
        {
            printf("There was an error while creating pipes.\n");
            return 0;
        }

        pid_t p = fork();
        if(p < 0)
        {
            printf("There was an error while forking a child.\n");
            return 0;
        }

        // reading end - fd[0]
        // writing end - fd[1]

        if(p == 0)
        {
            // Child1 process - run the first program
            close(fd[0]);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            execl("compute_sequence_1", "./compute_sequence_1", NULL, NULL);
        }
        else
        {
            p = fork();
            if(p == 0)
            {
                // Child2 process - run the second program
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                execl("compute_sequence_2", "./compute_sequence_2", NULL, NULL);
            }  
            else
            {
                close(fd[0]);
                close(fd[1]);
                wait(NULL);
                wait(NULL);
            } 
        }
    }
} 