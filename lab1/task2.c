/*
 * Task 2: Only 4 diners are allowed to attempt to grab the forks at the same time
 * Author: Louis Rozencwajg-Hays, Nicholas Gattuso III, Kexian Wu
 * I pledge my honor that I have abided by the Stevens Honor System.
 */

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define NUMP 5

pthread_mutex_t fork_mutex[NUMP];
pthread_mutex_t allow_to_grab_mutex;
pthread_mutex_t drop_mutex;
pthread_cond_t can_eat;

int curr_eating = 0;
int max_eating = 4;


int main()
{
  int i;
  pthread_t diner_thread[NUMP];
  int dn[NUMP];
  void *diner();
  for (i=0;i<NUMP;i++)
    pthread_mutex_init(&fork_mutex[i], NULL);

  pthread_mutex_init(&allow_to_grab_mutex, NULL);
  pthread_mutex_init(&drop_mutex, NULL);
  pthread_cond_init(&can_eat, NULL);

  for (i=0;i<NUMP;i++){
    dn[i] = i;
    pthread_create(&diner_thread[i],NULL,diner,&dn[i]);
  }
  for (i=0;i<NUMP;i++)
    pthread_join(diner_thread[i],NULL);

  for (i=0;i<NUMP;i++)
    pthread_mutex_destroy(&fork_mutex[i]);

  pthread_mutex_destroy(&allow_to_grab_mutex); //Destroy allow_to_grab_mutex mutex
  pthread_mutex_destroy(&drop_mutex); //Destroy drop_mutex mutex
  pthread_cond_destroy(&can_eat); //Destroy the condition variable
  pthread_exit(0);

}

void *diner(int *i)
{
  int v;
  int eating = 0;
  printf("I'm diner %d\n",*i);
  v = *i;
  while (eating < 5) {
    printf("%d is thinking\n", v);
    sleep( v/2);
    printf("%d is hungry\n", v);
    pthread_mutex_lock(&allow_to_grab_mutex); // Take allow_to_grab_mutex mutex
    while (curr_eating >= max_eating)
    {
        pthread_cond_wait(&can_eat, &allow_to_grab_mutex); // Wait until allowed to eat
    }
    ++curr_eating;
    pthread_mutex_unlock(&allow_to_grab_mutex); // Drop allow_to_grab_mutex mutex

    pthread_mutex_lock(&fork_mutex[v]);
    pthread_mutex_lock(&fork_mutex[(v+1)%NUMP]);

    printf("%d is eating\n", v);
    eating++;
    sleep(1);
    printf("%d is done eating\n", v);

    pthread_mutex_lock(&drop_mutex); // take the drop fork lock
    --curr_eating;
    pthread_mutex_unlock(&fork_mutex[v]);
    pthread_mutex_unlock(&fork_mutex[(v+1)%NUMP]);
    pthread_cond_signal(&can_eat); // signal one philosopher to eat
    pthread_mutex_unlock(&drop_mutex); // drop the drop fork lock
  }
  pthread_exit(NULL);
}

