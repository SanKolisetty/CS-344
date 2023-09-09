#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

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

int main(int argc, char * argv[])
{
    int n = 0;
    if(argc < 2) n = 1;
    else n = argc;
    if(n == 1)
    {
        sequence(100);
    }
    else
    {
        for(int i = 1; i < n; i++)
        {
            int k = atoi(argv[i]);
            sequence(k);
        }
    }
}