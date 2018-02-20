//only one hungry philosopher at a time should be able to attempt to eat.

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define NUMP 5


pthread_mutex_t grab_mutex;
pthread_mutex_t drop_mutex;

pthread_mutex_t fork_mutex[NUMP];

int main()
{
  int i;
  pthread_t diner_thread[NUMP];
  int dn[NUMP];
  void *diner();
  for (i=0;i<NUMP;i++)
    pthread_mutex_init(&fork_mutex[i], NULL);

  pthread_mutex_init(&grab_mutex, NULL);
  pthread_mutex_init(&drop_mutex, NULL);

  for (i=0;i<NUMP;i++){
    dn[i] = i;
    pthread_create(&diner_thread[i],NULL,diner,&dn[i]);
  }
  for (i=0;i<NUMP;i++)
    pthread_join(diner_thread[i],NULL);

  for (i=0;i<NUMP;i++)
    pthread_mutex_destroy(&fork_mutex[i]);

  pthread_mutex_destroy(&grab_mutex);
  pthread_mutex_destroy(&drop_mutex);

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
    pthread_mutex_lock(&grab_mutex); // Take the grab fork mutex
    pthread_mutex_lock(&fork_mutex[v]);
    pthread_mutex_lock(&fork_mutex[(v+1)%NUMP]);
    pthread_mutex_unlock(&grab_mutex); // Drop the grab fork mutex

    printf("%d is eating\n", v);
    eating++;
    sleep(1);
    printf("%d is done eating\n", v);

    pthread_mutex_lock(&drop_mutex); // take the drop fork lock
    pthread_mutex_unlock(&fork_mutex[v]);
    pthread_mutex_unlock(&fork_mutex[(v+1)%NUMP]);
    pthread_mutex_unlock(&drop_mutex); // drop the drop fork lock
  }
  pthread_exit(NULL);
}

