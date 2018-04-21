#include "linkedlist.h"
#include <stdio.h>
#include <iostream>

int main(int argc, char const *argv[]){
	//linkedlist List;
	// List.insertLast(1);
	// List.insertLast(2);
	// List.insertLast(3);
	// List.insertLast(4);

	// List.printElements();
	// printf("Finding Element:: %d \n", List.findElement(10));
	// List.insertAt(2,10);

	// List.printElements();

	// List.removeLast();
	// List.removeAt(0);
	// List.removeAt(0);

	// List.printElements();
	// printf("Finding Element:: %d \n", List.findElement(10));

	linkedlist list2;
	for(int x=0; x<5; x++){
		std::cout << "******* Trial: "<<x<<" *******" << std::endl;
		list2.insertLast(x);
		list2.printElements();
	}
	for(int x=0; x<5; x++){
		std::cout << "******* Trial: "<<x<<" *******" << std::endl;
		list2.removeLast();
		list2.printElements();
	}
	
	/* code */
	return 0;
}