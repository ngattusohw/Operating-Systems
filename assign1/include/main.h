#ifndef _MAIN_H_
#define _MAIN_H_

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

struct product {
	int pid;
  long time_produced;
  long time_inserted;
  long time_consumed;
  int life; //a positive integer number that is randomly generated
};


// gloal variables
extern long* turn_around_arr;
extern long* wait_time_arr;
extern int front;
extern int rear;
extern int numProducers;
extern int numConsumers;
extern int numProducts;
extern int scheduling;
extern int quantum;
extern int producedCount;
extern int consumedCount;
extern int productsInQueue;
extern pthread_mutex_t queue_lock;
extern pthread_cond_t queue_not_full;
extern pthread_cond_t queue_not_empty;
// Declare an array of products
extern struct product *products_queue;
extern int sizeOfQueue;


// all functions
int main(int argc, char** argv);
struct product* createProduct();
void *producer(void *);
void *consumer(void *);
void InitializeQueue(int size);
void pushQueue(struct product* product);
struct product* popQueue();
int fn(int num);
long getTimeStamp(void);
long maximum(long* arr, size_t size);
long minimum(long* arr, size_t size);
long average(long* arr, size_t size);

#endif
