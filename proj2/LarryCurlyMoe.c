#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define MAX 1000
#define NUM 5

sem_t holes;
sem_t full;
sem_t empty;
sem_t shovel;

int maxnum;
int curly = 0;

void *Larry(void *arg)
{
    int i = 0;
    while (i < maxnum)
    {
        sem_wait(&holes);
        sem_wait(&shovel);
        printf("Larry digs hole %d\n", i + 1);
        sleep(rand() % NUM);
        i++;
        sem_post(&shovel);
        sem_post(&empty);
    }
    return NULL;
}

void *Moe(void *arg)
{
    int i = 0;
    while (i < maxnum)
    {
        sem_wait(&empty);
        printf("Moe plants seed in hole %d\n", i + 1);
        sleep(rand() % NUM);
        i++;
        sem_post(&full);
    }
    return NULL;
}

void *Curly(void *arg)
{
    int i = 0;
    while (i < maxnum)
    {
        sem_wait(&full);
        sem_wait(&shovel);

        printf("Curly fills hole %d\n", i + 1);
        sleep(rand() % NUM);
        curly++;
        if (curly == maxnum)
        {
            exit(0);
        }
        i++;
        sem_post(&shovel);
        sem_post(&holes);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s Maxnum\n", argv[0]);
        exit(-1);
    }
    maxnum = atoi(argv[1]);
    if (maxnum > MAX)
    {
        printf("Max number of unfilled holes cannot exceed %d\n", MAX);
        exit(-1);
    }

    pthread_t larry_thread, moe_thread, curly_thread;

    sem_init(&holes, 0, maxnum);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, 0);
    sem_init(&shovel, 0, 1);

    pthread_create(&larry_thread, NULL, Larry, NULL);
    pthread_create(&moe_thread, NULL, Moe, NULL);
    pthread_create(&curly_thread, NULL, Curly, NULL);

    pthread_join(larry_thread, NULL);
    pthread_join(moe_thread, NULL);
    pthread_join(curly_thread, NULL);

    return 0;
}