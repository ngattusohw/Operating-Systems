#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"

struct node;
typedef struct node* Node;

struct node{
	Node next;
	int val;
};

struct llist{
	Node head;
	Node tail;
	int count;
};

Node makeNode(int v){
	Node returnNode = (Node) malloc(sizeof(struct node));
	returnNode->val = v;
	returnNode->next = NULL;
	return returnNode;
}

LList makeList(){
	LList returnList = (LList) malloc(sizeof(struct llist));
	returnList->head = returnList->tail = NULL;
	returnList->count = 0;
	return returnList;
}

//inserts a node with a value into the LinkedList
//PRECOND:: the_list is not null
void insertNode(LList the_list, int v){
	Node the_node = makeNode(v);
	Node hold;
	if(the_list->head == NULL){
		the_list->head = the_node;
		the_list->tail = the_node;
		the_list->count++;
	}else{
		hold = the_list->head;
		for(int x=0;x<the_list->count;x++){
			if(hold->val == v){
				printf("%s\n", "Trying to add a duplicate into the List..");
				return;
			}
			hold = hold->next;
		}
		//no reason why this should ever fire off.. just incase to avoid segfault
		if(hold==NULL){
			printf("%s\n", "BROKE AT INSERTION");
			return;
		}
		hold->next = the_node;
		the_list->tail = the_node;
	}
	return;
}

//Goal : remove a node with a certain val
//first step, find the val
//second, determine where it is
//third, remove it
//fourth, reassign pointers TAIL
//finally, adjust count value
//POSSIBLE EDGE CASES: there is nothing in the list, removing the head, removing from mid
//	the value is not in the list...
//PRECONDITION: the_list is not NULL
void removeNode(LList the_list, int v){
	Node hold = the_list->head;
	Node prev = hold;
	Node temp;
	if(the_list->count==0){
		printf("%s\n", "Trying to remove nothing from the list...");
		return;
	}else{
		//now determine where it is
		if(the_list->count==1 && the_list->head->val == v){
			//removing the head
			the_list->head = the_list->tail = NULL;
			the_list->count = 0;
		}else if(the_list->tail->val == v){
			//removing the tail..need to traverse through the list to get the prev of the tail
			for(int x=0;x<the_list->count-1;x++){
				hold = hold->next;
			}
			hold->next = NULL;
			the_list->tail = hold;
			the_list->count--;
		}else{
			//in the middle somewhere
			hold=hold->next;
			for(int x=0;x<the_list->count;x++){
				if(hold->val == v){
					temp = hold->next;
					prev->next = temp;
					hold = NULL; //possibly free hold? because theres no pointers to it
					the_list->count--;
					return;
				}else{
					prev = hold;
					hold = hold->next;
				}
			}
			printf("%s %i %s\n", "The value ", v , "is not in the list");
			return;
		}
	}
}


//A function that prints the contents of the_list
//PRECONDITION: the_list is not NULL
void printList(LList the_list){
	Node hold = the_list->head;
	for(int i = 0; i < the_list->count; ++i){
		printf("%i %s", hold->val, "->");
	}
	printf("END\n");
}

int getCount(LList the_list){
	return the_list->count;
}


/*
This implementation of reverse_list is not correct..
 */
// void reverse_list(LList the_list){
// 	Node currentNode = the_list->head->next; //this will not work??? wtf?
// 	head->next=null;
// 	Node hold_next=currentNode->next;
// 	Node hold_prev = the_list->head;

// 	while(hold_prev!=the_list->tail){
// 		hold_next = currentNode->next;
// 		currentNode->next = hold_prev;
// 		hold_prev = currentNode;
// 		currentNode = hold;
// 	}

// 	hold_next = the_list->head;
// 	the_list->head = the_list->tail;
// 	the_list->tail = hold_next;


// }
