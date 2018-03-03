#include "main.h"

// Initialize queue
void InitializeQueue(int size) {
  sizeOfQueue = size;
  products_queue = malloc(sizeof(struct product)*size);
}

// Pushes a product onto the queue
void pushQueue(struct product* product) {
  product->time_inserted = getTimeStamp();
  if (rear == sizeOfQueue - 1) {
    rear = -1;
  }
  products_queue[++rear] = *product;
	++productsInQueue;
}

//Removes a product from the queue and returns the removed product
struct product* popQueue() {
	struct product *removed = &products_queue[front++];
	if(front == sizeOfQueue) {
		front = 0;
	}
	--productsInQueue;
	return removed;
}
