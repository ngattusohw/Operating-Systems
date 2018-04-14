#include "main.h"

// Unload the old page and load the new page, setting valid bits accordingly
// Returns 1 if there is a swap, otherwise return 0
int loadPage(struct process* process, int pageNum, char *algorithm, int PagesEachProcess, unsigned long relativeTime) {
  int toSwap = -1;
  int i;
  int idx;
  int test = -1;
  if (!strcmp(algorithm,"LRU")) {
    // Cases 1.1: no need to swap and directly load into main memory
    if (process->numOfLoaded < PagesEachProcess) {
      process->pageTable->pages[pageNum]->lst_time_accessed = relativeTime;
      // Validates page
      process->pageTable->pages[pageNum]->valid = 1;
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
      // To find the oldest page (with the oldest timestamp) by looping through all pages loaded in memeory
      for (i = toSwap+1; i < process->pageTable->numOfPages; i++) {
        if (process->pageTable->pages[i]->valid &&
          process->pageTable->pages[i]->lst_time_accessed <
            process->pageTable->pages[toSwap]->lst_time_accessed) {
            // printf("i :: %ld \n", process->pageTable->pages[i]->lst_time_accessed);
            // printf("toSwap :: %ld \n", process->pageTable->pages[toSwap]->lst_time_accessed);
            toSwap = i;
        }
      }
      // Removes the oldest page
      process->pageTable->pages[toSwap]->valid = 0;
      // Inserts the new page
      process->pageTable->pages[pageNum]->valid = 1;
      process->pageTable->pages[pageNum]->lst_time_accessed = relativeTime;
      test = 1;
    }
  }


  else if (!strcmp(algorithm,"Clock")) {
    // Cases 2.1: no need to swap and directly load into main memory
    if (process->numOfLoaded < PagesEachProcess) {
      // Validates page
      process->pageTable->pages[pageNum]->valid = 1;
      pushClock(process, pageNum);
    }
    // Caes 2.2: need to swap pages
    else {
      // Get page index that is being unloaded
      idx = popClock(process);
      process->numOfLoaded--;
      if (idx != -1) {
        process->pageTable->pages[idx]->valid = 0;
      }
      // Inserts the new page
      process->pageTable->pages[pageNum]->valid = 1;
      pushClock(process, pageNum);
      test = 1;
    }
  }



  // algorithm -- FIFO
  else {
    // Case 3.1: no need to swap and directly load into main memory
    if (process->numOfLoaded < PagesEachProcess) {
      process->pageTable->pages[pageNum]->valid = 1;
      pushFIFO(process,pageNum);
    }
    // Caes 3.2: need to swap pages
    else {
      // Get page index that is being unloaded
      idx = popFIFO(process);
      process->numOfLoaded -= 1;

      if (idx != -1) {
        process->pageTable->pages[idx]->valid = 0;
      }
      process->pageTable->pages[pageNum]->valid = 1;
      pushFIFO(process, pageNum);
      test = 1;
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


// Returns page index that is being unloaded
int popFIFO(struct process* process) {
  int rtn = -1;
  struct loadedPage *temp;
  struct loadedPage *tempFirst;
  // Pop the first element for the linked list and update "loadedHead"
  if (process->loadedHead != NULL) {
    temp = process->loadedHead->next;
    tempFirst = process->loadedHead;
    rtn = tempFirst->pageNum;
    tempFirst->next = NULL;
    process->loadedHead = temp;
    free(tempFirst);
  }
  return rtn;
}


// Add to the end of the linked list
void pushClock(struct process* process, int pageNumber) {
  struct loadedPage *loadedPage;
  struct loadedPage *temp;
  loadedPage = (struct loadedPage*)calloc(1,sizeof(struct loadedPage));
  // Successfully allocate a new entry
  if (loadedPage != NULL) {
    loadedPage->pageNum = pageNumber;
    loadedPage->referenced = 1;
    loadedPage->next = NULL;
    // Set it as clock hand if "loadedHead" points to NULL
    if (process->loadedHead == NULL) {
      process->loadedHead = loadedPage;
    }
    // add it to the end of the linked list
    else {
      temp = process->loadedHead;
      while (temp->next != NULL) {
        temp = temp->next;
      }
      temp->next = loadedPage;
    }
  }
}

// Return the page index that is being unloaded
int popClock(struct process* process) {
  struct loadedPage *temp;
  struct loadedPage *tempLast;
  int toSwap = -1;
  temp = process->loadedHead;
  if (temp!=NULL) {
    // the case when the page has second chance
    while (1) {
      //if referenced num is one, set it to 0 and continue the loop
      if (temp->referenced > 0) {
        temp->referenced--;
        if (temp->next != NULL) {
          process->loadedHead = temp->next;
        }
        // add the page to the end of the linked plist
        tempLast = temp;
        while (tempLast->next != NULL) {
          tempLast = tempLast->next;
        }
        tempLast->next = temp;
        temp->next = NULL;
        temp = process->loadedHead;
      }
      // found the page to swap
      else {
        toSwap = temp->pageNum;
        if (temp->next != NULL) {
          process->loadedHead = temp->next;
        }
        temp->next = NULL;
        free(temp);
        break;
      }
    }
  }
  return toSwap;
}




// Checks if the page exists in the memory and resets the reference bit
int resetReferenceBit(struct process* process, int pageNum) {
    int rtn = -1;
    struct loadedPage *temp;
    if (process->loadedHead != NULL) {
        temp = process->loadedHead;
        while (temp->pageNum != pageNum && temp->next != NULL) {
          temp = temp->next;
        }
        if (temp->pageNum == pageNum) {
          temp->referenced = 1;
          rtn = 1;
        }
    }
    return rtn;
}
