#ifndef _MAIN_H_
#define _MAIN_H_

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define MAX_MEMEMORY_LOCATION 512
#define MAX_PAGE_SIZE 32

struct page {
    int pageNum;
    int valid;
    unsigned long lst_time_accessed;
};

struct pageTable {
  int numOfPages;
  struct page **pages;
};

struct loadedPage {
  int pageNum;
  int referenced;
  struct loadedPage *next;
};

struct process {
  int pid;
  int numOfLoaded;
  struct pageTable *pageTable;
  struct loadedPage *loadedHead;
  struct loadedPage *ClockHandPrev;
};



// All functions
int main(int argc, char** argv);
int isPowerOfTwo(unsigned int x);
unsigned long getTimeStamp(void);
struct pageTable *CreatePageTable(int TotalMemoryAllocation, int pageSize);
void FreePageTable(struct pageTable *pageTable);

void loadPage(struct process* process, int numOfPage, char *algorithm, int PagesEachProcess);
void pushFIFO(struct process* process, int pageNum);
void pushCLOCK(struct process* process, int pageNum);
int accessCLOCK(struct process* process, int pageNum);

#endif
