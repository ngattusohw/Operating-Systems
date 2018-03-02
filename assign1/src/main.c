#include "main.h"

// global variables
int front = 0;
int rear = -1;
long* turn_around_arr;
long* wait_time_arr;
int numProducers;
int numConsumers;
int numProducts;
int scheduling;
int quantum;
int producedCount = 0;
int consumedCount = 0;
int productsInQueue = 0;
int sizeOfQueue;
pthread_mutex_t queue_lock;
pthread_cond_t queue_not_full;
pthread_cond_t queue_not_empty;

// Declare an array of products
struct product *products_queue;

// return a new product
struct product* createProduct() {
	struct product* product = (struct product*)malloc(sizeof(struct product));
	if (product != NULL) {
		product->pid = ++producedCount;
		product->time_produced = getTimeStamp();
		product->life = random() % 1024;
	}
	return product;
}


int main (int argc, char** argv) {
  int i;
  int seed;
  int err;

  long start_time;
  long end_time;
  long start_producing;
  long end_producing;
  long start_consuming;
  long end_consuming;



  /*check arguments*/
  if (argc != 8) {
    printf("Error: Expected 7 arguments");
    return -1;
  }
  else {
    // Number of producer threads
    numProducers = atoi(argv[1]);
		if (numProducers <= 0) {
			printf("ERROR: Invalid number of producers");
			return -1;
		}
    // Number of consumer threads
    numConsumers = atoi(argv[2]);
    if (numConsumers <= 0) {
      printf("ERROR: Invalid number of consumers");
      return -1;
    }
    // Total number of products to be generated by all producer threads
    numProducts = atoi(argv[3]);
    if (numProducts <= 0) {
      printf("ERROR: Invalid number of products");
      return -1;
    }
    // Size of the queue to store products for both producer and consumer threads
    sizeOfQueue = atoi(argv[4]);
    if (sizeOfQueue < 0) {
      printf("ERROR: Invalid queue size");
      return -1;
    }
    // 0 or 1 for type of scheduling algorithm
    scheduling = atoi(argv[5]);
    if (scheduling == 0 && strcmp(argv[5], "0")==1 ) {
        printf("ERROR: Invalid value for scheduling algorithm");
        return -1;
    }
    if ((scheduling != 0) && (scheduling != 1)) {
        printf("ERROR: Invalid value for scheduling algorithm");
        return -1;
    }
    // Value of quantum used for round-robin scheduling
    quantum = atoi(argv[6]);
    if (quantum <= 0) {
      printf("ERROR: Invalid value for quantum");
      return -1;
    }
    // Seed for random number generator
    seed = atoi(argv[7]);
    if (seed < 0) {
      printf("ERROR: Invalid value for seed");
      return -1;
    }

    // Create pthreads
    pthread_t producers[numProducers];
    int pn[numProducers];
    pthread_t consumers[numConsumers];
    int cn[numConsumers];


    pthread_mutex_init(&queue_lock, NULL);
    pthread_cond_init(&queue_not_full, NULL);
    pthread_cond_init(&queue_not_empty, NULL);

    //Initialize the seed for random()
    srandom(seed);

    // Initialize queue

    if (sizeOfQueue > 0) {
      InitializeQueue(sizeOfQueue);
    }

    else {
      InitializeQueue(numProducts);
    }

    // initialize size of arrays
		turn_around_arr = (long*)malloc(sizeof(long)*numProducts);
		wait_time_arr = (long*)malloc(sizeof(long)*numProducts);


    // Start time
    start_time = getTimeStamp();

    // Start producing
    start_producing = getTimeStamp();

    //Generate the producer threads
    for (i = 0; i < numProducers; i++) {
      pn[i] = i + 1;
      err = pthread_create(&producers[i], NULL, &producer, &pn[i]);
    if (err) {
      printf("Error number: %i\n", err);
      }
    }
    // Start consuming
    start_consuming = getTimeStamp();
    //Generate the consumer threads
    for (i = 0; i < numConsumers; i++)
    {
      cn[i] = i + 1;
      err = pthread_create(&consumers[i], NULL, &consumer, &cn[i]);
      if (err) {
        printf("Error number: %i\n", err);
      }
    }


    //Join the producer threads
    for (i = 0; i < numProducers; i++) {
      pthread_join(producers[i], NULL);
    }
    // End producing
    end_producing = getTimeStamp();

    //Join the consumer threads
    for (i = 0; i < numConsumers; i++) {
      pthread_join(consumers[i], NULL);
    }
    // End consuming
    end_consuming = getTimeStamp();
    // End time
    end_time =  getTimeStamp();

    //Destroy mutexes and condition variables
    pthread_mutex_destroy(&queue_lock);
    pthread_cond_destroy(&queue_not_full);
    pthread_cond_destroy(&queue_not_empty);


    // print results
    printf("====================\n");
    printf("Performance Analysis\n");
    printf("====================\n");
    printf("Total Runtime: %ld\n", end_time - start_time);
    // turnaround times
		printf("Turnaround times\n Max: %ld milliseconds\n Min: %ld milliseconds\n Average: %ld milliseconds\n", maximum(turn_around_arr, sizeof(turn_around_arr)), minimum(turn_around_arr, sizeof(turn_around_arr)), average(turn_around_arr, sizeof(turn_around_arr)));
    // wait times
		printf("Wait times\n Max: %ld milliseconds\n Min: %ld milliseconds\n Average: %ld milliseconds\n", maximum(wait_time_arr, sizeof(wait_time_arr)), minimum(wait_time_arr, sizeof(wait_time_arr)), average(wait_time_arr, sizeof(wait_time_arr)));
    // producer throughput
    printf("Producer throughput: %ld items \n", (numProducts * 60000000/(end_producing - start_producing)));
    // consumer throughput
    printf("Consumer throughput: %ld items \n", (numProducts * 60000000/(end_consuming - start_consuming)));
  }
  return 0;
}
