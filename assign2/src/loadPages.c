#include "main.h"

// Unload the old page and load the new page, setting valid bits accordingly
// Returns 1 if there is a swap, otherwise return 0
int loadPage(struct process* process, int pageNum, char *algorithm, int PagesEachProcess) {
  int toSwap = -1;
  int i;
  int idx;
  int test = -1;
  if (!strcmp(algorithm,"LRU")) {
    // Cases 1.1: no need to swap and directly load into main memory
    if (process->numOfLoaded < PagesEachProcess) {
      process->pageTable->pages[pageNum]->lst_time_accessed = getTimeStamp();
      if (process->pageTable->pages[pageNum]->lst_time_accessed == -1) {
        printf("ERROR: Failed to get TimeStamp\n");
        return -1;
      }
    }
    // Caes 1.2: need to swap pages
    else {
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
            // printf("i :: %ld \n", process->pageTable->pages[i]->lst_time_accessed);
            // printf("toSwap :: %ld \n", process->pageTable->pages[toSwap]->lst_time_accessed);
            toSwap = i;
        }
      }
      // Removes the oldest page
      process->pageTable->pages[toSwap]->valid = 0;
      // Inserts the new page
      process->pageTable->pages[pageNum]->valid = 1;
      process->pageTable->pages[pageNum]->lst_time_accessed = getTimeStamp();
      // printf("%ld \n", process->pageTable->pages[pageNum]->lst_time_accessed);
      // Check error
      if (process->pageTable->pages[pageNum]->lst_time_accessed == -1) {
        printf("ERROR: Failed to get TimeStamp\n");
        fflush(stdout);
      }
      test = 1;
    }
  }


  // else if (!strcmp(algorithm,"CLOCK")) {
  //   // no need to swap when there are free emory Locations available for this process
  //     if (process->numOfLoaded < PagesEachProcess) {
  //         pushCLOCK(process, pageNum);
  //     }
  //     // Swaps pages
  //     else {
  //       struct loadedPage* ClockHandPrev = process->ClockHandPrev;
  //       struct loadedPage* ClockHand = process->ClockHandPrev->next;
  //       // int found = -1;
  //       struct loadedPage *temp;
  //       temp->pageNum = pageNum;
  //       temp->referenced = 1;
  //       temp->next = NULL;
  //       while (ClockHand!=NULL) {
  //         if (ClockHand->referenced) {
  //           ClockHand->referenced --;
  //           ClockHandPrev = ClockHand;
  //           if (ClockHand->next != NULL) {
  //             ClockHand = ClockHand->next;
  //           }
  //           else {
  //             ClockHand = process->loadedHead;
  //           }
  //         }
  //         else {
  //           temp->next = ClockHandPrev->next;
  //           ClockHandPrev->next = temp;
  //           process->ClockHandPrev = temp;
  //           // found = 1;
  //           break;
  //         }
  //   }
  // }
  // }
  // algorithm -- FIFO
  else {
    // Case 3.1: no need to swap and directly load into main memory
    if (process->numOfLoaded < PagesEachProcess) {
      process->pageTable->pages[pageNum]->valid = 1;
      pushFIFO(process,pageNum);
    }
    // Caes 3.2: need to swap pages
    else {
      test = 1;
      idx = popFIFO(process);
      if (idx != -1) {
        process->pageTable->pages[idx]->valid = 0;
      }
      process->pageTable->pages[pageNum]->valid = 1;
      pushFIFO(process, pageNum);
    }
  }
  process->numOfLoaded += 1;
  return test;
}


void pushFIFO(struct process* process, int pageNum) {
  struct loadedPage *loadedPage;
  struct loadedPage *temp;
  loadedPage = (struct loadedPage*)calloc(1,sizeof(struct loadedPage));
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



int popFIFO(struct process* process) {
  int rtn = -1;
  struct loadedPage *temp;
  if (process->loadedHead != NULL) {
    temp = process->loadedHead->next;
    rtn = process->loadedHead->pageNum;
    free(process->loadedHead);
    process->loadedHead = temp;
    process->numOfLoaded -= 1;
  }
  return rtn;
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
    }
    else {
      temp = process->ClockHandPrev;
      temp->next = loadedPage;
      process->ClockHandPrev = loadedPage;
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
