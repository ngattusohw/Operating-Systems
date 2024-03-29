// Kexian Wu (kwu5), Louis Rozencwajg-Hays (lrozencw), Nick Gattuso (ngattuso)
// I pledge my honor that I have abided by the Stevens Honor System.
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
};



// All functions
int main(int argc, char** argv);
int isPowerOfTwo(unsigned int x);
struct pageTable *CreatePageTable(int TotalMemoryAllocation, int pageSize);
void FreePageTable(struct pageTable *pageTable);
int loadPage(struct process* process, int numOfPage, char *algorithm, int PagesEachProcess, unsigned long relativeTime);
void pushFIFO(struct process* process, int pageNum);
int popFIFO(struct process* process);
void pushClock(struct process* process, int pageNumber);
int popClock(struct process* process);
int resetReferenceBit(struct process* process, int pageNum);

#endif
