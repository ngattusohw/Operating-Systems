#include "main.h"


// To check if the number is power of two
int isPowerOfTwo(unsigned int x) {
	// While x is even and > 1
	 while (((x & 1) == 0) && x > 1)  {
		 x >>= 1;
	 }
	 return (x == 1);
}



int main(int argc, char** argv) {
	int i, j, k;
	int found;
	int test;
	int processCount;
	char* plist;
	char* ptrace;
	FILE* plistfp; // file handle for plist file
	FILE* ptracefp; // file handle for ptrace file
	char* algorithm;
	int pageSize;
	int prePaging;
	int numProcess;
	int pid, TotalMemoryAllocation, MemoryLocation;
	int PagesEachProcess;
	int swapCount;
	int localPageNum;
	struct process** processes;
	unsigned long relativeTime; // Used for LRU algorithm



	// Check arguments
	if (argc != 6) {
		printf("Error: Expected 5 arguments");
		return -1;
	}
	else {
		plist = argv[1];
		ptrace = argv[2];
		pageSize = atoi(argv[3]);
		// Make sure page size is a positive number that is powers of 2, up to a max of 32
		if (pageSize <= 0 || pageSize>=MAX_PAGE_SIZE || !isPowerOfTwo((unsigned int) pageSize)) {
			printf("ERROR: Invalid value for page size\n");
			return -1;
		}

		if (strcmp(argv[4], "FIFO") == 0) {
			algorithm = "FIFO";
		}
		else if (strcmp(argv[4], "LRU") == 0) {
			algorithm = "LRU";
		}
		else if (strcmp(argv[4], "Clock") == 0)
			algorithm = "Clock";
		else {
			printf("ERROR: Invalid replacement algorithm\n");
			return 1;
		}

	  if (strcmp(argv[5], "+") == 0) {
			prePaging = 1;
		}
	  else if (strcmp(argv[5], "-") == 0) {
			prePaging = 0;
	  }
	  else {
			printf("ERROR: Invalid value for pre-paging\n");
	 		return -1;
		}
			// Reads from plist file
	   plistfp = fopen(plist, "r");
	   if (plistfp == NULL) {
	   		printf("ERROR: Unable to open plist file\n");
	   		return -1;
	   }

		 // printf("%s\n", "Successfully opened file");
		 // fflush(stdout);

	    numProcess = 0;
	    while(fscanf(plistfp, "%d %d", &pid, &TotalMemoryAllocation) != EOF) {
	    		numProcess += 1;
	    }
			// printf("%d \n", numProcess);
    	processes = NULL;
			if (numProcess > 0) {
				processes = (struct process**)calloc(numProcess, sizeof(struct process*));
			}
			if (processes == NULL) {
				printf("ERROR: Failed to allocate memory\n");
				return -1;
			}
			// Sets the file position to the beginning of the file
			rewind(plistfp);
			processCount = 0;
			while (fscanf(plistfp, "%d %d", &pid, &TotalMemoryAllocation) != EOF) {
				processes[processCount] = (struct process*) calloc(1, sizeof(struct process));
				processes[processCount]->pid = processCount;
				processes[processCount]->pageTable = CreatePageTable(TotalMemoryAllocation, pageSize);
				processes[processCount]->loadedHead = NULL;
				processes[processCount]->numOfLoaded = 0;
				processCount++;
			}
			fclose(plistfp);



		// Calculates how many pages on average each program should get into its assigned main memory part
		relativeTime = 1;
		// proportional allocation
		PagesEachProcess = MAX_MEMEMORY_LOCATION / pageSize / numProcess;
		// printf("%d\n", PagesEachProcess);
		for (i = 0; i<numProcess; i++) {
			for (j = 0; j<PagesEachProcess && j<processes[i]->pageTable->numOfPages; j++) {
				// Validates page
				processes[i]->pageTable->pages[j]->valid = 1;

				if (!strcmp(algorithm,"LRU")) {
					processes[i]->pageTable->pages[j]->lst_time_accessed = relativeTime;
					relativeTime++;
				}
				else if (!strcmp(algorithm,"FIFO")) {
					pushFIFO(processes[i],j);
				}
				else if (!strcmp(algorithm,"Clock")) {
					pushClock(processes[i],j);
				}
				processes[i]->numOfLoaded += 1;
			}
		}

		// Test
		// for (int i = 0; i < numProcess; ++i) {
		// 	printf("pid = %d num of loaded pages: %d\n",
		// 			processes[i]->pid,
		// 			processes[i]->numOfLoaded);
		// }

		// Reads from ptrace
		swapCount = 0;
	  ptracefp = fopen(ptrace, "r");
	  if (ptracefp == NULL)
	    {
	        printf("Unable to open ptrace file\n");
	        return 1;
	    }
		while(fscanf(ptracefp, "%d %d", &pid, &MemoryLocation) != EOF) {
			// printf("%s %d %d\n", "Start of while :: pid, memlocation ", pid, MemoryLocation);
			localPageNum = (int)ceil((double)MemoryLocation / pageSize);
			localPageNum -= 1; // translate to right index number

			// case 1: the page requested does not exist in the memory
			if (!processes[pid]->pageTable->pages[localPageNum]->valid) {
				test = loadPage(processes[pid],localPageNum, algorithm, PagesEachProcess, relativeTime);
				if (test == 1) {
					swapCount++;
				}
				if (!strcmp(algorithm,"LRU")) {
					relativeTime ++;
				}
				// load next page
				found = -1;
				// the case when prePaging is TRUE: brings 2 pages into memory for every swap
				//
				if (prePaging) {
					test = -1;
					// Finds the next contiguous page that does not in the memory
					for (k = localPageNum+1; k < processes[pid]->pageTable->numOfPages; k++) {
						if (!processes[pid]->pageTable->pages[k]->valid) {
							test = loadPage(processes[pid],k, algorithm, PagesEachProcess,relativeTime);
							found = 1;
							break;
						}
					}
					if (found == -1) {
						for (k = 0; k < processes[pid]->pageTable->numOfPages; k++) {
							if (!processes[pid]->pageTable->pages[k]->valid) {
								test = loadPage(processes[pid],k, algorithm, PagesEachProcess, relativeTime);
								break;
							}
						}
					}
					if (!strcmp(algorithm,"LRU")) {
						relativeTime ++;
					}
				}
			}
			// Case 2: the page requested exists in the memory
			else {
				if (!strcmp(algorithm,"LRU")) {
					processes[pid]->pageTable->pages[localPageNum]->lst_time_accessed = relativeTime;
					relativeTime++;
				}
				else if (!strcmp(algorithm,"Clock")) {
					found = resetReferenceBit(processes[pid], localPageNum);
					if (found == -1) {
						printf("ERROR: Failed to reset the value of variable referenced\n");
						return -1;
					}
				}
				// For FIFO algorithm, do nothing when the page already exists in the memory
			}
		} // end of While loop
	  fclose(plistfp);
		// Free memory
		if (processes != NULL) {
			for (i = 0; i < numProcess; i++) {
				FreePageTable(processes[i]->pageTable);
				free(processes[i]->loadedHead);
				free(processes[i]);
			}
			free(processes);
		}


		// print total number of page swaps
		printf("Total number of page swaps: %d\n", swapCount);
		fflush(stdout);

		return 0;
	} // end of arguments else
} // end of main
