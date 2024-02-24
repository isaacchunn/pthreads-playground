#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 10
//If producer higher, more is produced than consumer on average. vice versa.

#define PRODUCER_TIME 1
#define CONSUMER_TIME 10
struct Shared
{
    //Char buffer
    char buf[BUFFER_SIZE];
    //Buffer to indicate if buffer ready to be written for producers
    sem_t buffer;
    //Semaphore to indicate if data ready to read by consumers
    sem_t data;
    //Semaphore for multiple producers and consumers
    sem_t mutex;
    //Shared integers to determine the in to produce and out to read.
    int in;
    int out;
} shared;

//Create a shared structure to define this problem
struct Shared shared;

/// @brief Prints out the values of shared buffer
/// @param idx the idx of in and out pointers
void PrintBuffer(int idx)
{
    //Print out the buffer
    printf("[");
    for(int i = 0; i < BUFFER_SIZE; i++)
    {
        printf(" %c ", shared.buf[i]);
    }
    printf("]\n");

    //Then print out the idx pointer position
    printf("  ");
    for(int i = 0; i < idx; i++)
    {
        printf("   ");
    }
    printf("^\n");
}


/// @brief A function to call for producers to produce items
/// @param item //The item to put in
void Put(char item)
{
    int buffValue;
    /*
    Write an item into the shared buffer, if there is no empty buffer then wait
    */
    printf("Producer is waiting for shared.buffer semaphore.\n");
    
    //Wait if theres a consumer using the buffer
    sem_wait(&shared.buffer);
    //MULTIPLE PRODUCERS FOR EG
    sem_wait(&shared.mutex);
    
    sem_getvalue(&shared.buffer, &buffValue);
    printf("shared.buffer: %d, Produced %c to be put at in: %d\n", buffValue, item, shared.in);

    //Update the item in the buffer with this in
    shared.buf[shared.in] = item;

    PrintBuffer(shared.in);

    //Increment the number of data now available in the buffer
    shared.in = (shared.in + 1) % BUFFER_SIZE;

    //Generate a signal to indicate that data can be written
    sem_post(&shared.mutex);
    sem_post(&shared.data);
    printf("Producer has indicated that data exists by shared.data semaphore.\n");
}

/// @brief A function for consumers to get items
void Get()
{
    int item, data;
    /*Prepare the data from bufer, if there is no data - wait*/
    //Wait for producer to produce something so wait on that semaphore
    printf("Consumer is waiting for shared.data semaphore.\n");
    sem_wait(&shared.data);
    sem_getvalue(&shared.data, &data);
    //Have data, get it
    item = shared.buf[shared.out];

    printf("shared.data: %d, Acquiring %c at out: %d\n", data, item, shared.out);

    PrintBuffer(shared.out);

    //increment the buffer now available
    shared.out = (shared.out + 1) % BUFFER_SIZE;

    //Signal that buffer can now be written on
    sem_post(&shared.buffer);
    printf("Consumer has indicated that data exists by shared.buffer semaphore.\n");
}

void *tProducer()
{
    while(1)
    {
        sleep(PRODUCER_TIME);
        //Put in a random character between 'a' to 'z'
        Put((char)97 + rand() % 26);
    }
}

void *tConsumer()
{  
    while(1)
    {
        sleep(CONSUMER_TIME);
        Get();
    }
}

int main()
{
    //Initialzie time
    srand(time(NULL));
    pthread_t tP, tC;

    sem_init(&shared.data, 0, 0);
    sem_init(&shared.buffer, 0, BUFFER_SIZE);

    //Initialize other variables
    shared.in = 0;
    shared.out = 0;

    //Create two threads
    pthread_create(&tP, NULL, &tProducer, NULL);
    pthread_create(&tP, NULL, &tConsumer, NULL);

    //Join both threads
    pthread_join(tP,NULL);
    pthread_join(tC,NULL);

    //Destroy both semaphores
    sem_destroy(&shared.data);
    sem_destroy(&shared.buffer);

    return 0;
}