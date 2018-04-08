#include "main.h"



int isPowerOfTwo(unsigned int x) {
	// While x is even and > 1
	 while (((x & 1) == 0) && x > 1)  {
		 x >>= 1;
	 }
	 return (x == 1);
}


// Get the current time in milliseconds
unsigned long getTimeStamp(void) {
	struct timeval tv;
	long long milliseconds = -1;
	gettimeofday(&tv, NULL);
	milliseconds = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ;
	return milliseconds;
}


int main(int argc, char** argv) {
	int i, j, k, found;
	char* plist;
	char* ptrace;
	FILE* plistfp; // file handle for plist file
	FILE* ptracefp; // file handle for ptrace file
	char* algorithm;
	int pageSize;
	int prePaging;
	int numProcess;
	int pid, TotalMemoryAllocation, MemoryLocation;
	int totalPages;
	int PagesEachProcess;
	int swapCount;
	int numOfPage;
	struct process** processes;



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
		else if (strncmp(argv[4], "CLOCK", 4) == 0)
			algorithm = "CLOCK";
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

	    numProcess = 0;
	    totalPages = 0;
	    while(fscanf(plistfp, "%d %d", &pid, &TotalMemoryAllocation) != EOF) {
	    		totalPages += (int)ceil((double)TotalMemoryAllocation / pageSize);
	    		numProcess += 1;
	    }
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
		while(fscanf(plistfp, "%d %d", &pid, &TotalMemoryAllocation) != EOF) {
			for (i = 0;i<numProcess;i++) {
				processes[i] = (struct process*) calloc(1, sizeof(struct process));
				processes[i]->pid = i;
				processes[i]->pageTable = CreatePageTable(TotalMemoryAllocation, pageSize);
				processes[i]->loadedHead = NULL;
				processes[i]->ClockHandPrev = NULL;
			}
		}
		fclose(plistfp);
		// Calculates how many pages each program should get into its assigned main memory part
		PagesEachProcess = MAX_MEMEMORY_LOCATION / pageSize / numProcess;
		for (i = 0;i<numProcess;i++) {
			for (j = 0;j<PagesEachProcess && j<processes[i]->pageTable->numOfPages; j++) {
				// Validates page
				processes[i]->pageTable->pages[j]->valid = 1;
				if (!strcmp(algorithm,"LRU")) {
					processes[i]->pageTable->pages[j]->lst_time_accessed = getTimeStamp();
					if (processes[i]->pageTable->pages[j]->lst_time_accessed == -1) {
						printf("ERROR: Failed to get TimeStamp\n");
						return -1;
					}
				}
				else if (!strcmp(algorithm,"FIFO")) {
					pushFIFO(processes[i],j);
				}
				else if (!strcmp(algorithm,"CLOCK")) {
					pushCLOCK(processes[i],j);
				}
			}
		}
		// Reads from ptrace
		swapCount = 0;
	    ptracefp = fopen(ptrace, "r");
	    if (ptracefp == NULL)
	    {
	        printf("Unable to open ptrace file\n");
	        return 1;
	    }
	    int numOfTimesRun = 0;
		while(fscanf(ptracefp, "%d %d", &pid, &MemoryLocation) != EOF) {
			printf("%s %d %d\n", "Start of while :: pid, memlocation " ,pid, MemoryLocation);
			numOfPage = MemoryLocation;
			// case 1: the page requested does not exist in the memory
			printf("%s\n", "THE ISSUE IS HERE!!!!");
			printf("TESTING ::  %d\n", processes[pid]->pageTable->numOfPages);
			printf("NUM OF PAGE %d\n", numOfPage);
			if (processes[pid]->pageTable->pages[numOfPage] && !processes[pid]->pageTable->pages[numOfPage]->valid) {
				swapCount++;
				loadPage(processes[pid],numOfPage, algorithm, PagesEachProcess);
				// load next page
				found = -1;
				if (prePaging) {
					// Finds the next contiguous page that does not in the memory
					for (k = numOfPage+1; k < processes[pid]->pageTable->numOfPages; k++) {
						if (!processes[pid]->pageTable->pages[k]->valid) {
							loadPage(processes[pid],k, algorithm, PagesEachProcess);
							found = 1;
							break;
						}
					}
					if (found == -1) {
						for (k = 0; k < processes[pid]->pageTable->numOfPages; i++) {
							if (!processes[pid]->pageTable->pages[k]->valid) {
								loadPage(processes[pid],k, algorithm, PagesEachProcess);
							}
						}
					}
				}
			} // end of if

			// Case 2: the page requested exists in the memory
			else {
				printf("%s\n", "In else!");
				if (!strcmp(algorithm,"LRU")) {
					printf("%s\n", "Inside of LRU");
					processes[pid]->pageTable->pages[numOfPage]->lst_time_accessed = getTimeStamp();
					if (processes[pid]->pageTable->pages[numOfPage]->lst_time_accessed == -1) {
						printf("ERROR: Failed to get TimeStamp\n");
						return -1;
					}
				}
				else if (!strcmp(algorithm,"CLOCK")) {
					found = accessCLOCK(processes[numOfPage], j);
					if (found == -1) {
						printf("ERROR: Failed to reset the value of variable referenced\n");
						return -1;
					}
				}
				// For FIFO algorithm, do nothing when the page already exists in the memory
			}
			numOfTimesRun++;
			printf("%s %d\n", "End of while " , numOfTimesRun);
		} // end of While loop
		printf("%s\n", "Hello!");

	    fclose(ptracefp);
		// Free memory
		if (processes != NULL) {
			for (i = 0; i < numProcess; i++) {
				FreePageTable(processes[i]->pageTable);
				free(processes[i]->loadedHead);
				free(processes[i]->ClockHandPrev);
				free(processes[i]);
			}
			free(processes);
		}
		return 0;
	} // end of arguments else
} // end of main
