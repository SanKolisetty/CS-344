// To compile
// gcc -fno-stack-protector q3.c
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

int puzzle[9][9];
int col_validity[9];
int row_validity[9];
int subgrid_validity[9];

typedef struct
{
    int row;
    int column;
}parameters;

int getid(int x, int y)
{
    if(x == 0 && y == 0) return 0;
    if(x == 0 && y == 3) return 1;
    if(x == 0 && y == 6) return 2;
    if(x == 3 && y == 0) return 3;
    if(x == 3 && y == 3) return 4;
    if(x == 3 && y == 6) return 5;
    if(x == 6 && y == 0) return 6;
    if(x == 6 && y == 3) return 7;
    if(x == 6 && y == 6) return 8;
}

void * checking_rows(void * arg)
{
    parameters * startpos = (parameters *)malloc(sizeof(parameters));
    startpos = arg;
    
    int i = startpos->row;
    int present[9] = {0};

    for(int j = startpos->column; j < 9; j++)
    {
        present[puzzle[i][j]-1]++;
    }

    int flag = 0;
    for(int j = 0; j < 9; j++)
    {
        if(present[j] != 1)
        {
            row_validity[i] = 0;
            flag = 1;
            break;
        }
    }

    if(flag == 0)
    {
        row_validity[i] = 1;
    }
}

void * checking_columns(void * arg)
{
    parameters * startpos = (parameters *)malloc(sizeof(parameters));
    startpos = arg;

    int j = startpos->column;
    
    int present[9] = {0};
    for(int i = 0; i < 9; i++)
    {
        present[puzzle[i][j]-1]++;
    }

    int flag = 0;
    for(int i = 0; i < 9; i++)
    {
        if(present[i] != 1)
        {
            col_validity[j] = 0;
            flag = 1;
            break;
        }
    }

    if(flag == 0)
    {
        col_validity[j] = 1;
    }   
}

void * checking_subgrids(void * arg)
{
    parameters * startpos = (parameters *)malloc(sizeof(parameters));
    startpos = arg;

    int present[9] = {0};

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            present[puzzle[startpos->row+i][startpos->column+j]-1]++;
        }
    }

    int flag = 0;
    for(int i = 0; i < 9; i++)
    {
        if(present[i] != 1)
        {
            subgrid_validity[getid(startpos->row, startpos->column)] = 0;
            flag = 1;
        }
    }

    if(flag == 0)
    {
        subgrid_validity[getid(startpos->row, startpos->column)] = 1;
    }
}

int main()
{
    FILE *fp = fopen("week10-ML2-input1.txt", "r");
    for(int i = 0; i < 9; i++)
    {
        for(int j = 0; j < 9; j++)
        {
            fscanf(fp, "%d", &puzzle[i][j]);
        }
    }
    fclose(fp);
    
    pthread_t check_rows[9];
    pthread_t check_cols[9];
    pthread_t check_subgrids[9];

    parameters rows_send[9];
    parameters cols_send[9];
    parameters subgrid_send[9];

    for(int i = 0; i < 9; i++)
    {
        rows_send[i].row = i;
        rows_send[i].column = 0;
        pthread_create(&check_rows[i], NULL, checking_rows, &rows_send[i]);
    }

    for(int i = 0; i < 9; i++)
    {
        cols_send[i].row = 0;
        cols_send[i].column = i;
        pthread_create(&check_cols[i], NULL, checking_columns, &cols_send[i]);
    }

    int start_subgridx = 0;
    int start_subgridy = 0;
    
    int count = 0;

    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            subgrid_send[count].row = start_subgridx;
            subgrid_send[count].column = start_subgridy;
            pthread_create(&check_subgrids[count], NULL, checking_subgrids, &subgrid_send[count]); 
            start_subgridy += 3;
            count++;
        }
        start_subgridx += 3;
        start_subgridy = 0;
    }

    for(int i = 0; i < 9; i++)
    {
        pthread_join(check_subgrids[i], NULL);
    }

    for(int i = 0; i < 9; i++)
    {
        pthread_join(check_rows[i], NULL);
    }

    for(int i = 0; i < 9; i++)
    {
        pthread_join(check_cols[i], NULL);
    }

    for(int i = 0; i < 9; i++)
    {
        if(col_validity[i] == 0)
        {
            printf("The given sudoku is not valid.\n");
            exit(0);
        }
    }

    for(int i = 0; i < 9; i++)
    {
        if(row_validity[i] == 0)
        {
            printf("The given sudoku is not valid.\n");
            exit(0);
        }
    }

    for(int i = 0; i < 9; i++)
    {
        if(subgrid_validity[i] == 0)
        {
            printf("The given sudoku is not valid.\n");
            exit(0);
        }
    }

    printf("The given sudoku is valid.\n");
    return 0;
}