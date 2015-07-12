#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

struct node{
  int data;
  struct node *prev;
  struct node *next;
};

struct node *head = NULL;
struct node *tail = NULL;

pthread_mutex_t queueSizeMutex;
pthread_cond_t itemAddedCondition;

int noMoreWork = 0;

void queue_create() {
  pthread_mutex_init(&queueSizeMutex, NULL);
  pthread_cond_init(&itemAddedCondition, NULL);
}

void queue_destroy() {
  pthread_mutex_destroy(&queueSizeMutex);
  pthread_cond_destroy(&itemAddedCondition);
}

void queue_add(int newData) {
  struct node *newItem = malloc(sizeof *newItem);
  newItem->data = newData;
  newItem->next = NULL; 
  newItem->prev = NULL;

  pthread_mutex_lock(&queueSizeMutex);
  printf("Locked in queue_add()\n");
  if(head == NULL) {
    head = tail = newItem;
  } else {
    head->prev = newItem;
    newItem->next = head;
    head = newItem;    
  }
 
  assert(head != NULL);
  assert(tail != NULL);
  
 
  pthread_cond_broadcast(&itemAddedCondition);
  printf("Unlcoking in queue_add()\n");
  pthread_mutex_unlock(&queueSizeMutex);
}

int queue_remove() {
  struct node *item = NULL;
  
  pthread_mutex_lock(&queueSizeMutex);
 printf("Locked in queue_remove()\n");

  while(tail == NULL && noMoreWork == 0) {
    printf("queue_remove(): no work waiting, but noMoreWork==0\n");
    pthread_cond_wait(&itemAddedCondition, &queueSizeMutex);
  }

  if(tail == NULL && noMoreWork == 1) {
    pthread_mutex_unlock(&queueSizeMutex);
    return -1;
  }

  item = tail;
  tail = item->prev;

  if(tail == NULL) {
    head = NULL;
  }
  
  printf("unlcoking in queue_remove()\n");

  pthread_mutex_unlock(&queueSizeMutex);
   
  int data = item->data;
  free(item);
  return data;
}

void *producer(void *t) {
  long threadId = (long)t;
  int i;

  printf("Producer: %ld\n", threadId);

  for(i = 0; i < 10; i++) {
    int data = rand();
    printf("Producing data: %d\n", data);
    queue_add(data);
    printf("Added %d to the queue\n", data);
    sleep(1);
  }

  noMoreWork = 1;

  pthread_cond_broadcast(&itemAddedCondition);  

  printf("Finished producing work. Producer %ld exiting.\n", threadId);

  pthread_exit(NULL);
}  

void *consumer(void *t) {
  long threadId = (long)t;
  int data;

  printf("Consumer: %ld\n", threadId);

  while(0 <= (data = queue_remove())) {
    printf("Consumed %d\n", data);
  }

  printf("No more work to be done. Consumer %ld exiting \n", threadId);

  pthread_exit(NULL);
}

int main() {
  int i;
  int data1 = 50;
  int data2 = 33;
  long t1=1, t2=2, t3=3;
  pthread_t threads[3];
  pthread_attr_t attr;

  queue_create();

  //queue_add(&data1);
  //queue_add(&data2);

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_create(&threads[1], &attr, consumer, (void *)t2);
  pthread_create(&threads[2], &attr, consumer, (void *)t3);
  pthread_create(&threads[0], &attr, producer, (void *)t1);

  //printf("queue1: %d\n", queue_remove());
  //printf("queue2: %d\n", queue_remove());

  for(i=0; i<3; i++) {
    pthread_join(threads[i], NULL);
  }

  if(head != NULL) {
    printf("DIDN'T CONSUME THE WHOLE QUEUE!\n");
  }

  queue_destroy();

  pthread_attr_destroy(&attr);

  pthread_exit(NULL);
} 
