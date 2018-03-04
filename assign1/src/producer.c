#include "main.h"

void *producer(void *i) {
  int producer_id = *((int*)i);
  struct product* product;
  // Continue to produce when the current number of products does not reach the total number of products
  while (producedCount < numProducts) {
    // Only 1 thread can have the mutex lock at a time
    pthread_mutex_lock(&queue_lock);

    // When product count is less than the max number of products, producers are allowed to produce products if queue is not full, otherside they need to wait
    while (producedCount < numProducts && productsInQueue>=sizeOfQueue) {
      pthread_cond_wait(&queue_not_full, &queue_lock);
    }
    // Checks product count to decide which clause to go
    if (producedCount < numProducts){
        product = createProduct();
        pushQueue(product);
        printf("Producer %d has produced product %d\n", producer_id, product->pid);
        fflush(stdout);
        pthread_cond_broadcast(&queue_not_empty);
        pthread_mutex_unlock(&queue_lock);
        usleep(100000);
    }
    // When enough products have been produced, an else clause will be executed for those who are still waiting
    else {
      pthread_cond_broadcast(&queue_not_empty);
      pthread_mutex_unlock(&queue_lock);
      break;
    }
  }
  return NULL;
}
