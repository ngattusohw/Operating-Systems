#include "main.h"

void *producer(void *i) {
  int producer_id = *((int*)i);
  struct product* product;
  // Continue to produce when the current number of products does not reach the total number of products
  while (producedCount < numProducts) {
    // Only 1 thread can have the mutex lock at a time
    pthread_mutex_lock(&queue_lock);
    if (producedCount >= numProducts) {
      pthread_mutex_unlock(&queue_lock);
      break;
    }
    while (producedCount < numProducts && productsInQueue>=sizeOfQueue) {
      pthread_cond_wait(&queue_not_full, &queue_lock);
    }
    if (producedCount < numProducts){
        product = createProduct();
        pushQueue(product);
        printf("Producer %d has produced product %d\n", producer_id, product->pid);
    }
    fflush(stdout);
    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&queue_lock);
    usleep(100000);
  }
  return NULL;
}
