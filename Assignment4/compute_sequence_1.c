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

void sequence(int k)
{
    printf("%d ", k);
    while (k > 1)
    {
        if (k % 2 == 0)
        {
            printf("%d ", k / 2);
            k = k / 2;
        }
        else
        {
            printf("%d ", 3 * k + 1);
            k = 3 * k + 1;
        }
    }
    printf("\n");
}

int main()
{
    int n = 100; 
    sequence(n);
}