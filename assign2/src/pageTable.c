// Kexian Wu (kwu5), Louis Rozencwajg-Hays (lrozencw), Nick Gattuso (ngattuso)
// I pledge my honor that I have abided by the Stevens Honor System.
#include "main.h"

struct pageTable *CreatePageTable(int TotalMemoryAllocation, int pageSize) {
	int i;
	int numOfPages;
	struct pageTable *tempPageTable;
	tempPageTable = (struct pageTable*)calloc(1,sizeof(struct pageTable));
	if (tempPageTable != NULL) {
		numOfPages = (int)ceil((double)TotalMemoryAllocation/pageSize);
		tempPageTable->pages = (struct page**)calloc(numOfPages, sizeof(struct page*));
		if (tempPageTable->pages != NULL) {
			tempPageTable->numOfPages = numOfPages;
			for (i = 0;i<numOfPages;i++) {
				struct page *tempPage = (struct page*)calloc(1,sizeof(struct page));
				tempPage->pageNum = i;
				tempPage->valid = 0;
				tempPageTable->pages[i] = tempPage;
			}
		}
	}
	return tempPageTable;
}


void FreePageTable(struct pageTable *pageTable) {
	if (pageTable != NULL) {
		int i;
		for (i = 0;i < pageTable->numOfPages;i++) {
			if (pageTable->pages[i] != NULL) {
				free(pageTable->pages[i]);
			}
		}
		free(pageTable->pages);
		free(pageTable);
	}
}
