#include "main.h"

// Get the current time in milliseconds
long getTimeStamp(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long milliseconds = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
  return milliseconds;
}

// return the maximum value of the array
long maximum(long* arr, size_t size) {
  int i = 1;
  long max = arr[i];
  while (i < size) {
    if (arr[i] > max) {
      max = arr[i];
    }
    i++;
  }
  return max;
}
// return the minimum value of the array
long minimum(long* arr, size_t size) {
  int i = 1;
  long min = arr[i];
  while (i < size) {
    if (arr[i] < min) {
      min = arr[i];
    }
    i++;
  }
  return min;
}

// return the average value of the array
long average(long* arr, size_t size) {
   int i = 0;
   long sum = 0;
   long avg;
   while (i < size) {
     sum = sum + arr[i];
     i++;
   }
   avg = sum/size;
   return avg;
 }
