#include "main.h"

// Get the current time in milliseconds
long long getTimeStamp(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long long milliseconds = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
  return milliseconds;
}

// return the maximum value of the array
long long maximum(long long* arr, size_t size) {
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
long long minimum(long long* arr, size_t size) {
  int i = 1;
  long long min = arr[0];
  while (i < size) {
    if (arr[i] < min) {
      min = arr[i];
    }
    i++;
  }
  return min;
}

// return the average value of the array
long long average(long long* arr, size_t size) {
   int i = 0;
   long long sum = 0;
   long long avg;
   while (i < size) {
     sum = sum + arr[i];
     i++;
   }
   avg = sum/size;
   return avg;
 }
