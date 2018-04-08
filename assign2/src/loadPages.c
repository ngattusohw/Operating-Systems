#include "main.h"

// Unload the old page and load the new page, setting valid bits accordingly
void loadPage(struct process* process, int numOfPage, char *algorithm, int PagesEachProcess) {
  int toSwap = -1;
  int i;
  if (!strcmp(algorithm,"LRU")) {
    // To find first valid page
    for (i = 0; i < process->pageTable->numOfPages; i++) {
      if (process->pageTable->pages[i]->valid) {
        toSwap = i;
        break;
      }
    }
    // To find the oldest page (with the smallest timestamp)
    for (i = toSwap+1; i < process->pageTable->numOfPages; i++) {
      if (process->pageTable->pages[i]->valid &&
        process->pageTable->pages[i]->lst_time_accessed < process->pageTable->pages[toSwap]->lst_time_accessed) {
          toSwap = i;
      }
    }
    // Removes the oldest page
    process->pageTable->pages[toSwap]->valid = 0;
    // Inserts the new page
    process->pageTable->pages[numOfPage]->valid = 1;
    process->pageTable->pages[numOfPage]->lst_time_accessed = getTimeStamp();
    if (process->pageTable->pages[numOfPage]->lst_time_accessed == -1) {
      printf("ERROR: Failed to get TimeStamp\n");
    }
  }
  else if (!strcmp(algorithm,"CLOCK")) {
    
  }
  // algorithm -- FIFO
  else {

  }



}


void pushFIFO(struct process* process, int pageNum) {
  struct loadedPage *loadedPage;
  struct loadedPage *temp;
  loadedPage = (struct loadedPage*)calloc(1,sizeof(struct loadedPage*));
  if (loadedPage != NULL) {
    loadedPage->pageNum = pageNum;
    loadedPage->next = NULL;
    if (process->loadedHead == NULL) {
      process->loadedHead = loadedPage;
    }
    else {
      temp = process->loadedHead;
      while (temp->next != NULL) {
        temp = temp->next;
      }
      temp->next = loadedPage;
    }
  }
}


void pushCLOCK(struct process* process, int pageNum) {
  struct loadedPage *loadedPage;
  struct loadedPage *temp;
  loadedPage = (struct loadedPage*)calloc(1,sizeof(struct loadedPage*));
  if (loadedPage != NULL) {
    loadedPage->pageNum = pageNum;
    loadedPage->referenced = 1;
    loadedPage->next = NULL;
    if (process->loadedHead == NULL) {
      process->loadedHead = loadedPage;
      process->ClockHandPrev = loadedPage;
      process->numOfLoaded += 1;
    }
    else {
      temp = process->ClockHandPrev;
      temp->next = loadedPage;
      process->ClockHandPrev = loadedPage;
      process->numOfLoaded += 1;
    }
  }
}




// Checks if the page exists in the memory
int accessCLOCK(struct process* process, int pageNum) {
    int rtn = -1;
    struct loadedPage *temp;
    if (process->loadedHead != NULL) {
        temp = process->loadedHead;
        while (temp->pageNum != pageNum && temp->next != NULL) {
          temp = temp->next;
        }
        if (temp->pageNum == pageNum) {
          temp->referenced += 1;
          rtn = 1;
        }
    }
    return rtn;
}
