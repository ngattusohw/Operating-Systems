#include "main.h"
// You can analyze the behavior by seeing how many products are consumed by each consumer
void *consumer(void *i) {
  int consumer_id = *((int*)i);
  struct product* taken_product;
	int idx;
  while (consumedCount < numProducts) {
    pthread_mutex_lock(&queue_lock);
    while (consumedCount < numProducts && productsInQueue == 0) {
      pthread_cond_wait(&queue_not_empty, &queue_lock);
    }
    long long now;
    if (consumedCount < numProducts) {
      // FCFS
      if (scheduling == 0) {
        taken_product = popQueue();
        now = getTimeStamp();
        wait_time_arr[taken_product->pid-1] += now - taken_product->time_inserted;
        idx = 0;
        while (idx < taken_product->life) {
          fn(10);
          idx++;
        }
        now = getTimeStamp();
        turn_around_arr[taken_product->pid-1] = now-taken_product->time_produced;
        ++consumedCount;
        printf("Consumer %d has consumed product %d\n", consumer_id, taken_product->pid);
        fflush(stdout);
        pthread_cond_broadcast(&queue_not_full);
      }
      // round robin
      else {
        taken_product = popQueue();
        now = getTimeStamp();
        wait_time_arr[taken_product->pid-1] += now - taken_product->time_inserted;
        if (taken_product->life > quantum)  {
          for (idx = quantum; idx  > 0; idx--) {
            fn(10);
          }
          taken_product->life = taken_product->life - quantum;
          pushQueue(taken_product);
        }
        else {
          while (idx < taken_product->life) {
            fn(10);
            idx++;
          }
          now = getTimeStamp();
          turn_around_arr[taken_product->pid-1] = now - taken_product->time_produced;
          ++consumedCount;
          printf("Consumer %d has consumed product %d\n", consumer_id, taken_product->pid);
          fflush(stdout);
          pthread_cond_broadcast(&queue_not_full);
        }
      }
      pthread_mutex_unlock(&queue_lock);
      usleep(100000);
    }
    else {
      pthread_cond_broadcast(&queue_not_empty);
      pthread_mutex_unlock(&queue_lock);
      break;
    }
  }
  return NULL;
}
