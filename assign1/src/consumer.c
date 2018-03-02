#include "main.h"
// You can analyze the behavior by seeing how many products are consumed by each consumer
void *consumer(void *i) {
  int consumer_id = *((int*)i);
  struct product* taken_product;
	int idx;
  while (consumedCount < numProducts) {
    // printf("%s\n", "Poop 1");
    fflush(stdout);
    pthread_mutex_lock(&queue_lock);
    if (consumedCount >= numProducts) {
      pthread_mutex_unlock(&queue_lock);
      // printf("%s\n", "Poop 2 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
      // fflush(stdout);
      break;
    }
    while (consumedCount < numProducts && productsInQueue == 0) {
      printf("%s %d %d %d\n", "Inside of pthread_cond_wait while loop,consumedCount, numProducts consumer id = " , consumedCount, numProducts, consumer_id);
      fflush(stdout);
      pthread_cond_wait(&queue_not_empty, &queue_lock);
      printf("%s %d\n", "After cond_wait for consumder id ==  " , consumer_id);
      fflush(stdout);
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
          // printf("%s\n", "ass");
          // fflush(stdout);
          fn(10);
          idx++;
        }
        now = getTimeStamp();
        turn_around_arr[taken_product->pid-1] = now-taken_product->time_produced;
        ++consumedCount;
        printf("Consumer %d has consumed product %d CONSUMED COUNT %d\n", consumer_id, taken_product->pid, consumedCount);
        fflush(stdout);
        pthread_cond_signal(&queue_not_full);
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
          pthread_cond_signal(&queue_not_full);
        }
      }
      pthread_mutex_unlock(&queue_lock);
      // printf("%s %d\n", " I AM ABOUT TO SLEEP!" , consumer_id);
      // fflush(stdout);
      usleep(100000);
      // printf("%s %d %d %d\n", " I AM DONE SLEEPING id, consumedCount, numProducts" , consumer_id, consumedCount, numProducts);
      // fflush(stdout);
    }else{
      printf("%s %d %d %d\n", "Consumed count = num products!! :: consumer_id == " , consumedCount, numProducts, consumer_id);
      fflush(stdout);
      pthread_cond_broadcast(&queue_not_empty);
      //return NULL;
      break;
    }
  }
  printf("%s %d\n", "The consumers are done? !!!!!!!!!!!!!!!!!!!!!", consumer_id);
  return NULL;
}
