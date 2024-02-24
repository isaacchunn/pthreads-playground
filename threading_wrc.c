#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int c = 0; //A global variable

//Create a mutex
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; // or use pthread_mutex_init to lock it

//Some function to increment global variable c
void fnC(int id)
{
    int i, j;
    for (i = 0; i < 10; i++)
    {
        //Lock the mutex to see if we can get control of it. Granted access if no other thread owns the mutex to become owner of the mutex
        pthread_mutex_lock(&mutex1);
        c++; //GLOBAL VARIABLE IS INCREMENTED with no locks or semaphores, its going to have race conditions
        for(j = 0; j < 5000000; j++)
        {
            //printf(" %d", c);
            //fflush(stdout);
        }
        printf("Thread %d: %d\n",id, c);
        //Unlock the mutex when we are done, and must be the owner of the mutex
        pthread_mutex_unlock(&mutex1);
    }
}

int main()
{
    pthread_t tA, tB;
    int rtA, rtB;

    /*Create two thread*/
    if((rtA = pthread_create(&tA, NULL, fnC, 1)))
        printf("Thread creation failed: %d\n", rtA);

    if((rtB = pthread_create(&tB, NULL, fnC, 2)))
        printf("Thread creation failed: %d\n", rtB);

    /*
    Wait for both threads to finish
    */
   pthread_join(tA, NULL);
   pthread_join(tB, NULL);

   //Destroy our mutex as no longer required
   pthread_mutex_destroy(&mutex1);
}
