#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

char* outFlag = "";
int total_threads;
int available_threads;
int total_tasks = -1;

//Message node
typedef struct message_node {
    long long msg;
    struct message_node* next;
} MessageNode;

//Message queue - a singly linked list
//Remove from head, add to tail
typedef struct {
    MessageNode* head;
    MessageNode* tail;
    pthread_mutex_t mutex;
    
    //Add a condition variable
    pthread_cond_t cond;
} MessageQueue;
//----

void updateAvailableThreadCounter(MessageQueue* q, int x) {
    pthread_mutex_lock(&q->mutex);
    available_threads += x;
    pthread_mutex_unlock(&q->mutex);
}

//Create a queue and initilize its mutex and condition variable
MessageQueue* createMessageQueue()
{
    MessageQueue* q = (MessageQueue*)malloc(sizeof(MessageQueue));
    q->head = q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    
    //Initialize the condition variable
    pthread_cond_init(&q->cond, NULL);
    return q;
}

//"Send" a message - append it onto the queue
void sendMessage(MessageQueue* q, long long val) {
    //Modified version from examples
    MessageNode* node = (MessageNode*)malloc(sizeof(MessageNode));
    node->msg = val;
    node->next = NULL;

    // critical section
    pthread_mutex_lock(&q->mutex);
    if (q->tail != NULL) {
        q->tail->next = node;       // append after tail
        q->tail = node;
    } else {
        q->tail = q->head = node;   // first node
    }

    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

//"Receive" a message - remove it from the queue
int getMessage(MessageQueue* q, long long* msg_out) {
    //Modified version from examples
    int success = 0;

    MessageNode* oldHead = q->head;
    *msg_out = oldHead->msg;    // copy out the message
    q->head = oldHead->next;
    if (q->head == NULL) {
        q->tail = NULL;         // last node removed
    }
    free(oldHead);
    success = 1;

    return success;
}


//---
void* workerFunc(void* arg);

#define NUM_THREAD_ARGS 2
#define MSG_PER_WORKER 10

//Thread arguments
typedef struct {
    int workerId;
    MessageQueue* q;
} ThreadArgs;
//---

//** How it should work for realsies */
/* Set up all threads and go to sleep on cond wait, 
   push all tasks onto queue, when something is pushed onto queue send out cond var, some thread will eat it up
// If all threads are busy they won't notice cond var, so when a thread finishes have it check if the queue is empty
   if it isn't then take a task
// Once queue is empty AND available_thread_num is back at the starting value you can exit/free threads
*/ 
int main(int argc, char* argv[]) {
    char filename[30];
    char line[256];
    long long iterTrack = 0;

    if (argc > 2) {
        outFlag = argv[2];
    }
    strcpy(filename, argv[1]);

    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file\n");
        exit(0);
    }

    int lines = 0;
    while(fgets(line, sizeof(line), fp)) {
        lines++;
    }
    fclose(fp);
    total_tasks = lines-1;

    fp = fopen(filename, "r");
    fscanf(fp, "%d\n", &total_threads);
    available_threads = total_threads;

    pthread_t tid[total_threads];
    ThreadArgs args[NUM_THREAD_ARGS];
    int i;
    MessageQueue* q = createMessageQueue();

    // create worker threads
    if (total_tasks < total_threads) {total_threads = total_tasks;}
    for (i = 0; i < total_threads; i++) {
        args[i].workerId = i + 1;
        args[i].q = q;
        pthread_create(&tid[i], NULL, workerFunc, &args[i]);
    }

    long long iterNum = 0;
    while (!feof(fp)) {
        //push task to queue
        fscanf(fp, "%lld\n", &iterNum);
        iterTrack += iterNum;
        sendMessage(args->q, iterNum);
    }
    fclose(fp);

    for (i = 0; i < total_threads; i++) {
        pthread_join(tid[i], NULL);
    }
    
    //Remember to delete the message queue
    free(q);

    if (strcmp(outFlag, "csv") == 0) {
        printf("%lld,", iterTrack);//, (float)(end - start) / CLOCKS_PER_SEC); CLOCK ISN'T ACCURATE D:
    }
    return 0;
}

void* workerFunc(void* arg)
{
    ThreadArgs* args = (ThreadArgs*)arg;
    int track = 1;

    if(total_tasks < args->workerId) {
        // free(args);
        pthread_exit(0);
    }

    while(track == 1) {
        pthread_mutex_lock(&args->q->mutex);
        while (args->q->head == NULL) {
            if (total_tasks == 0) {
                pthread_cond_broadcast(&args->q->cond);
                pthread_mutex_unlock(&args->q->mutex);
                // return NULL;
                // free(args);
                pthread_exit(0);
            }

            pthread_cond_wait(&args->q->cond, &args->q->mutex);

        }
    if (args->q->head != NULL) {
        total_tasks--;
        
        long long msgVal;
        getMessage(args->q, &msgVal);

        pthread_mutex_unlock(&args->q->mutex);

        // pi algorithm
        long double factor = 1.0;
        long double sum = 0.0;
        long double pi;
        for (long long j = 0; j < msgVal; j++, factor = -factor ) {
            sum += factor / (2 * j+1);
        }
        pi = 4.0 * sum;

        // if outFlag == 0 or 1 etc.. need to check if they want output
        if (strcmp(outFlag, "true") == 0) {
            fprintf(stderr, "Thread %d completed computed Pi using %lld iterations, the result is %.15Lf\n", args->workerId, msgVal, pi);
        
        } 
    }

    }
    return NULL;
}