#include "main.h"

void *producer(void *i) {
  int producer_id = *((int*)i);
  struct product* product;
  // Continue to produce when the current number of products does not reach the total number of products
  while (producedCount < numProducts) {
    // printf("%d %d %s\n", producedCount, numProducts, "Produced and numProducts");
    // fflush(stdout);
    // Only 1 thread can have the mutex lock at a time
    pthread_mutex_lock(&queue_lock);
    if (producedCount >= numProducts) {
      printf("%s %d\n", "ProducedCount >= numProducts, producer id = " , producer_id);
      fflush(stdout);
      pthread_mutex_unlock(&queue_lock);
      break;
    }
    while (producedCount < numProducts && productsInQueue>=sizeOfQueue) {
      printf("%s %d\n", "Producer inside pthread_cond_wait while loop --> producer id", producer_id);
      fflush(stdout);
      pthread_cond_wait(&queue_not_full, &queue_lock);
    }
    if (producedCount < numProducts){
        product = createProduct();
        pushQueue(product);
        printf("Producer %d has produced product %d\n", producer_id, product->pid);
        fflush(stdout);
        pthread_cond_signal(&queue_not_empty);
        pthread_mutex_unlock(&queue_lock);
        usleep(100000);
    }else{
      pthread_cond_broadcast(&queue_not_empty);
      pthread_mutex_unlock(&queue_lock);
      break;
    }

  }
  printf("%s %d\n", "The producers are done? ", producer_id);
  fflush(stdout);
  return NULL;
}
