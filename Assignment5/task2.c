#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
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

struct batsmen
{
    char name[100];
    char bowler[100];
    int num_of_balls;
    int num_of_runs;
    int out;
};

int main()
{
    // ftok to generate unique key
    key_t key = ftok("shmfile",65);

    // shmget returns an identifier in shmid
    int shmid = shmget(key,900000,0666|IPC_CREAT);

    // shmat to attach to shared memory
    char *str = (char*) shmat(shmid,(void*)0,0);

    int num_of_lines = atoi(str);
    str += strlen(str) + 1;

    struct batsmen arr[12];
    int num_of_batsmen = 0;
    int ct = 0;

    while(num_of_lines)
    {
        char check[1024];
        strncpy(check, str, strlen(str)+1);

        char *temp = strtok(check, " ");
        int ct = 0;

        char batsman[100];
        char bowl[100];
        char number[100];
        char isRuns[100];

        while (temp != NULL && ct <= 5)
        {   
            if(ct == 1)
            {
                strcpy(bowl,temp);
            }
            else if(ct == 3)
            {
                strcpy(batsman,temp);
            }
            else if(ct == 4)
            {
                strcpy(number, temp);
            }
            else if(ct == 5)
            {
                int flag = 0;
                for(int i = 0; i < num_of_batsmen; i++)
                {
                    if(strcmp(arr[i].name, batsman) == 0)
                    {
                        flag = 1;
                        arr[i].num_of_balls++;
                        
                        if(strcmp(number, "FOUR," ) == 0)
                        {
                            arr[i].num_of_runs += 4;
                        }
                        else if(strcmp(number, "SIX,") == 0)
                        {
                            arr[i].num_of_runs += 6;
                        }
                        else if(strcmp(number, "OUT,") == 0)
                        {
                            strcpy(arr[i].bowler, bowl);
                            arr[i].out = 1;
                        }
                        else if(strcmp(temp,"runs,") == 0 || strcmp(temp,"run,") == 0 || strcmp(temp,"run") == 0 || strcmp(temp, "runs") == 0)
                        {
                            if(strcmp(number,"1") == 0)
                            {
                                arr[i].num_of_runs++;
                            }
                            else if(strcmp(number,"2") == 0)
                            {
                                arr[i].num_of_runs += 2;
                            }
                            else if(strcmp(number, "3") == 0)
                            {
                                arr[i].num_of_runs += 3;
                            }
                            else if(strcmp(number, "5") == 0)
                            {
                                arr[i].num_of_runs += 5;
                            }
                        }
                        else if(strcmp(temp, "wide,") == 0)
                        {
                            arr[i].num_of_balls--;
                        }
                        break;
                    }
                }
                if(flag == 0)
                {
                    strcpy(arr[num_of_batsmen].name,batsman);
                    arr[num_of_batsmen].num_of_balls = 1;
                    arr[num_of_batsmen].num_of_runs = 0;
                    arr[num_of_batsmen].out = 0;
                    strcpy(arr[num_of_batsmen].bowler, "*");

                    if(strcmp(number, "FOUR," ) == 0)
                    {
                        arr[num_of_batsmen].num_of_runs += 4;
                    }
                    else if(strcmp(number, "SIX,") == 0)
                    {
                            arr[num_of_batsmen].num_of_runs += 6;
                    }
                    else if(strcmp(number, "OUT,") == 0)
                    {
                        strcpy(arr[num_of_batsmen].bowler, bowl);
                        arr[num_of_batsmen].out = 1;
                    }
                    else if(strcmp(temp,"runs,") == 0 || strcmp(temp,"run,") == 0)
                    {
                        if(strcmp(number,"1") == 0)
                        {
                            arr[num_of_batsmen].num_of_runs++;
                        }
                        else if(strcmp(number,"2") == 0)
                        {
                            arr[num_of_batsmen].num_of_runs += 2;
                        }
                        else if(strcmp(number, "3") == 0)
                        {
                            arr[num_of_batsmen].num_of_runs += 3;
                        }
                        else if(strcmp(number, "5") == 0)
                        {
                            arr[num_of_batsmen].num_of_runs += 5;
                        }
                    }
                    else if(strcmp(temp, "wide,") == 0)
                    {
                        arr[num_of_batsmen].num_of_balls--;
                    }
                    num_of_batsmen++;
                }
            }
            temp = strtok(NULL, " ");
            ct++;
        }
        num_of_lines--;
        str += strlen(str)+1;
    }

    for(int i = 0; i < num_of_batsmen; i++)
    {
        if(arr[i].out)
        {
            char * end = arr[i].name + strlen(arr[i].name) - 1;
            end--; 
            *(end + 1) = NULL;
            printf("%s b %s %d %d\n", arr[i].name, arr[i].bowler, arr[i].num_of_runs, arr[i].num_of_balls);
        }
        else
        {
            char * end = arr[i].name + strlen(arr[i].name) - 1;
            end--; 
            *(end + 1) = NULL;
            printf("%s %s %d %d\n", arr[i].name, arr[i].bowler, arr[i].num_of_runs, arr[i].num_of_balls);
        }
    }

    //detach from shared memory
    shmdt(str);

    // destroy the shared memory
    shmctl(shmid,IPC_RMID,NULL);

    return 0;
}