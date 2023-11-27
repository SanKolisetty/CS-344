// To compile
// gcc q2.c -lm
// To run
// ./a.out

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <math.h>
#include <wait.h>
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

int image_x;
int image_y;

int kxm,kxn;
int kym,kyn;

int kernelx[100][100];
int kernely[100][100];
int image[500][500];

struct msg
{
    long int msg_type;
    int row_number;
    int column_number;
};

int matmult(int kernel[100][100], int r, int c, int kx, int ky)
{
    int sum = 0;

    int r1 = r-1;
    int c1 = c-1;

    int r2 = r+1;
    int c2 = c+1;

    for(int i = r1; i <= r2; i++)
    {
        for(int j = c1; j <= c2; j++)
        {
            sum += image[i][j]*kernel[i-r+1][j-c+1];
        }
    }
    return sum;
}

void sorting()
{
    FILE *f = fopen("210101093_output.txt", "r");
    double matrix[256][256];
    for(int i = 0; i < 256*256; i++)
    {
        int x, y; double val;
        fscanf(f, "%d %d %lf", &x,&y, &val);
        matrix[x-1][y-1] = val;
    }
    fclose(f);
    f = fopen("210101093_output.txt", "w");
    for(int i = 0; i < 256; i++)
    {
        for(int j = 0; j < 256; j++)
        {
            fprintf(f, "Edge value at (%d, %d) is %f\n", i+1, j+1, matrix[i][j]);
        }
    }
    fclose(f);
}

int main(void)
{
    struct msg buf;
    int msqid;
    key_t key;

    FILE *f = fopen("ML2-input.txt", "r");
    fscanf(f, "%d %d\n", &kxm, &kxn);
    // printf("%d %d\n", kxm, kxn);

    for(int i = 0; i < kxm; i++)
    {
        for(int j = 0; j < kxn; j++)
        {
            fscanf(f, "%d", &kernelx[i][j]);
        }
    }

    // for(int i = 0; i < kxm; i++)
    // {
    //     for(int j = 0; j < kxn; j++)
    //     {
    //         printf("%d ", kernelx[i][j]);
    //     }
    // }

    fscanf(f, "%d %d\n", &kym, &kyn);
    // printf("%d %d\n", kym, kyn);

    for(int i = 0; i < kym; i++)
    {
        for(int j = 0; j < kyn; j++)
        {
            fscanf(f, "%d", &kernely[i][j]);
        }
    }

    // for(int i = 0; i < kym; i++)
    // {
    //     for(int j = 0; j < kyn; j++)
    //     {
    //         printf("%d ", kernely[i][j]);
    //     }
    // }

    fscanf(f, "%d %d", &image_x, &image_y);
    // printf("%d %d\n", image_x, image_y);

    for(int i = 0; i < image_x; i++)
    {
        for(int j = 0; j < image_y; j++)
        {
            fscanf(f, "%d", &image[i][j]);
        }
    }

    // for(int i = 0; i < image_x; i++)
    // {
    //     for(int j = 0; j < image_y; j++)
    //     {
    //         printf("%d ", image[i][j]);
    //     }
    // }

    fclose(f);

    FILE *f1 = fopen("210101093_output.txt", "w");

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

    int child_working = 0;
    for (int i = 1; i <= 256*256; i++)
    { 
        if(child_working >= 8)
        {
            wait(NULL);
            child_working--;
        }
        if (msgrcv(msqid, (struct msgbuf *)&buf, sizeof(buf), 0, 0) == -1)
        {
            perror("msgrcv");
            exit(1);
        }
        pid_t pid = fork();
        child_working++;
        if(pid == 0)
        {
            int x = matmult(kernelx, buf.row_number, buf.column_number, kxm, kxn);
            int y = matmult(kernely, buf.row_number, buf.column_number, kym, kyn);
            double G = (double)x*x + (double)y*y;
            G = sqrt(G);
            if(G >= 255.0) G = 255;
            fprintf(f1, "%d %d %f\n", buf.row_number, buf.column_number, G);
            printf("%d %d is received from the queue.\n", buf.row_number, buf.column_number);
            exit(0);
        }
    }

    while(child_working > 0)
    {
        wait(NULL);
        child_working--;
    }

    fclose(f1);
    sorting();

    printf("Everything done!\n");
}