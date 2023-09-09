#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

void sequence(int k)
{
    printf("n = %d: ", k);
    printf("%d ", k);
    while(k > 1)
    {
        if(k%2 == 0) 
        {
            printf("%d ", k/2);
            k = k/2;
        }
        else
        {
            printf("%d ", 3*k+1);
            k = 3*k+1;
        }
    }
    printf("\n");
}

int main()
{
    int n = 0;
    char * val = getenv("n");
    if(getenv("n") == NULL)
    {
        n = 1;
        sequence(100);
    }
    else
    {
        char temp[strlen(val)+1];
        int j = 0;
        for(int i = 0; i < strlen(val); i++)
        {
            if(val[i] == ' ')
            {
                temp[j] = '\0';
                int k = atoi(temp);
                sequence(k);
                bzero(&temp, sizeof(temp));
                j = -1;
            }
            else
            {
                temp[j] = val[i];
            }
            j++;
        }
        int k = atoi(temp);
        sequence(k);
    }
}