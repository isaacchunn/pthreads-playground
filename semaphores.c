#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int c = 0;
sem_t sem_mutex; //i.e a mutex semaphore

void fnC(char k, int p)
{
    int i , temp;
    for(i = 0; i < p; i++)
    {
        sem_wait(&sem_mutex);
        printf("Entered %c, c = %d\n", k , c);
        temp = c;
        temp = temp + 1;
        c = temp;
        sem_post(&sem_mutex);
    }
}

void *tAf() {fnC('A', 1000000);}
void *tBf() {fnC('B', 500000);}

int main()
{
    int rtA, rtB;
    pthread_t tA, tB;

    //Initialize mutex
    sem_init(&sem_mutex, 0 ,1 ); // initial value = 1
    /*
    Create two threads, A and B
    */
    if((rtA = pthread_create(&tA, NULL, &tAf, NULL)))
        printf("Thread 1 failed to create");

    if((rtB = pthread_create(&tB, NULL, &tBf, NULL)))
        printf("Thread 2 failed to create");

    /*
    Wait for both threads to finish
    */
    pthread_join(tA, NULL);
    pthread_join(tB, NULL);
    sem_destroy(&sem_mutex);

    return 0;
}