#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_PEOPLE 5
#define NUM 5

sem_t nojudge, mutex;
sem_t all_checked_in, confirmed;

int entered, checked = 0;
int judge_count = 0;
int ids[MAX_PEOPLE] = {0};

void *Immigrant(void *arg)
{
    int id = *(int *)arg;
    sem_wait(&nojudge);
    // enter
    printf("Immigrant %d enters.\n", id);
    entered++;
    sem_post(&nojudge);
    // check in
    sem_wait(&mutex);
    printf("Immigrant %d checks in.\n", id);
    sleep(rand() % NUM);
    checked++;

    int i = 0;
    while (ids[i] != 0)
        i++;
    ids[i] = id;

    if (judge_count == 1 && checked == entered)
        sem_post(&all_checked_in);
    else
        sem_post(&mutex);
    // sit down
    printf("Immigrant %d sits down.\n", id);
    // confirm
    sem_wait(&confirmed);
    // swear
    printf("Immigrant %d swears.\n", id);
    sleep(rand() % NUM);
    // get certificate
    printf("Immigrant %d gets certificate.\n", id);
    sleep(rand() % NUM);
    // leave
    sem_wait(&nojudge);
    printf("Immigrant %d leaves.\n", id);
    sem_post(&nojudge);
}

void *Spectator(void *arg)
{
    int id = *(int *)arg;
    sem_wait(&nojudge);
    printf("Spectator %d enters.\n", id);
    sem_post(&nojudge);
    // spectator
    printf("Spectator %d spectates.\n", id);
    sleep(rand() % NUM);

    printf("Spectator %d leaves.\n", id);
}

void *Judge(void *arg)
{
    int id = *(int *)arg;
    sem_wait(&nojudge);
    sem_wait(&mutex);
    // enter
    printf("Judge %d enters.\n", id);
    judge_count = 1;
    if (entered > checked)
    {
        sem_post(&mutex);
        sem_wait(&all_checked_in);
    }
    // confirm
    for (int i = 0; ids[i] != 0; i++)
    {
        printf("Judge %d confirms the immigrant %d.\n", id, ids[i]);
        ids[i] = 0;
    }
    sleep(rand() % NUM);
    sem_post(&confirmed);

    // leave
    entered = 0, checked = 0;
    printf("Judge %d leaves.\n", id);
    judge_count = 0;
    sem_post(&mutex);
    sem_post(&nojudge);
}

int main(int argc, char **argv)
{

    srand(time(NULL));

    pthread_t immigrants[MAX_PEOPLE];
    pthread_t spectators[MAX_PEOPLE];
    pthread_t judges[MAX_PEOPLE];

    sem_init(&nojudge, 0, 1);
    sem_init(&mutex, 0, 1);
    sem_init(&all_checked_in, 0, 0);
    sem_init(&confirmed, 0, 0);

    // sequential mode
    for (int i = 0; i < MAX_PEOPLE; i++)
    {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&(immigrants[i]), NULL, &Immigrant, id);
        sleep(rand() % NUM);
        pthread_create(&(judges[i]), NULL, &Judge, id);
        sleep(rand() % NUM * 2);
        pthread_create(&(spectators[i]), NULL, &Spectator, id);
        sleep(rand() % NUM);
    }
    for (int i = 0; i < MAX_PEOPLE; i++)
    {
        pthread_join(immigrants[i], NULL);
        pthread_join(judges[i], NULL);
        pthread_join(spectators[i], NULL);
    }

    return 0;
}