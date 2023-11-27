#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <math.h>
#include <wait.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <syslog.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <pthread.h>
#define size 400
#define share 100 // share = size/4

// Matrices
int mat1[size][size];
int mat2[size][size];
int result[size][size];

// Lock
pthread_mutex_t mutexsum;

// Initializing matrices
void initialize()
{
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            mat1[i][j] = rand()%100 + 1;
        }
    }
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            mat2[i][j] = rand()%100 + 1;
        }
    }
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            result[i][j] = 0;
        }
    }
}

// Function to print matrices to a file
void mat_to_file()
{
    FILE *fp1 = fopen("matrix1.txt", "w");
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            fprintf(fp1, "%d ",mat1[i][j]);
        }
        fprintf(fp1, "\n");
    }
    printf("Matrix 1 successfully printed in the file matrix1.txt!\n");
    fclose(fp1);

    FILE *fp2 = fopen("matrix2.txt", "w");
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            fprintf(fp2, "%d ", mat2[i][j]);
        }
        fprintf(fp2, "\n");
    }
    printf("Matrix 2 successfully printed in the file matrix2.txt!\n");
    fclose(fp2);
    
}

// Function to multiply
void * Multiply(void * arg)
{   
    int row = *((int *)arg);
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            int sum = 0;
            for(int k = row; k < row + share; k++)
            {
                sum += mat1[i][k]*mat2[k][j];
            }
            pthread_mutex_lock(&mutexsum);
            result[i][j] += sum;
            pthread_mutex_unlock(&mutexsum);
        }
    }
    pthread_exit((void *) 0);
}

int main()
{
    initialize();
    pthread_mutex_init(&mutexsum, NULL);
    pthread_t threads[4];
    int x[4];
    for(int i = 0; i < 4; i++)
    {
        x[i] = i*share;
        pthread_create(&threads[i], NULL, Multiply, &(x[i]));
    }

    for(int i = 0; i < 4; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // For printing the matrices in to a file
    mat_to_file();

    pthread_mutex_destroy(&mutexsum);
    FILE *fp = fopen("output.txt", "w");
    for(int i = 0; i < size; i++)
    {
        for(int j = 0; j < size; j++)
        {
            fprintf(fp, "%d ", result[i][j]);
        }
        fprintf(fp, "\n");
    }
    printf("Result successfully printed in the file output.txt!\n");
    fclose(fp);
    pthread_exit(NULL);
}