#define _XOPEN_SOURCE 700
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
    syslog(LOG_INFO, "n = %d: ", k);
    syslog(LOG_INFO, "%d ", k);
    while (k > 1)
    {
        if (k % 2 == 0)
        {
            syslog(LOG_INFO, "%d ", k / 2);
            k = k / 2;
        }
        else
        {
            syslog(LOG_INFO, "%d ", 3 * k + 1);
            k = 3 * k + 1;
        }
    }
    syslog(LOG_INFO, "\n");
}

void start_daemon()
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
        perror("Can't get file limit");

    if ((pid = fork()) < 0)
        perror("Can't fork");
    else if (pid != 0)
        exit(0);

    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        perror("Can't ignore SIGHUP");
    if ((pid = fork()) < 0)
        perror("Can't fork");
    else if (pid != 0) 
        exit(0);

    FILE *fp = NULL;
    fp = fopen("daemonpid", "w+");
    int x = getpid();
    fprintf(fp, "%d\n", x);
    fclose(fp);

    openlog("start_daemon", LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "Daemon has been created successfully.\n");

    if (chdir("/") < 0)
        perror("Can't change directory to /");

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    while (1)
    { 
        pid_t x = fork();
        if (x < 0)
        {
            syslog(LOG_ERR, "There was some error while creating a child of the daemon process.\n");
            exit(0);
        }
        else if (x == 0)
        {
            sequence(getpid());
            exit(0);
        }
        else
        {
            wait(NULL);
        }
        sleep(20);
    }
}

void stop_daemon()
{
    FILE *fp = fopen("daemonpid", "r");
    int daemonkill;
    if (fscanf(fp, "%d", &daemonkill) == 1)
    {

    }
    kill(daemonkill, SIGTERM);
    syslog(LOG_INFO, "The daemon was killed.\n");
    fclose(fp);
}

int main(int argc, char *argv[])
{ 
    // to run - gcc 210101093.c -0 run_daemon
    // to start - ./run_daemon start
    // to stop - ./run_daemon stop
    // to check the proces - ps -efj
    // to read the log - cat /var/log/syslog
    
    if (argc < 2)
    {
        printf("Please provide the necessary arguments.\n");
        exit(0);
    }

    if (strcmp(argv[1], "start") == 0)
    {
        printf("Starting...\n");
        start_daemon();
    }
    else if (strcmp(argv[1], "stop") == 0)
    {
        printf("Ending...\n");
        stop_daemon();
    }
    else
    {
        printf("Please provide the correct arguments.\n");
        exit(0);
    }
    return 0;
}