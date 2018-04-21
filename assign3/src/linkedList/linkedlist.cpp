#include "linkedlist.h"
#include <stdio.h>

linkedlist::linkedlist(){
	head = nullptr;
	tail = nullptr;
	size = 0;
}


linkedlist::~linkedlist(){
	printf("%s\n", "Being descructed Linked");
	for(int x=0;x<size-1;x++){
		removeLast();
	}
	printf("%s\n", "Done being deconstructed");
	head = nullptr;
	tail = nullptr;
}

linkedlist::Node::Node(){
	next = nullptr;
	elem = 0;
}

linkedlist::Node::~Node(){
	printf("%s\n", "Deconstructing node");
}


void linkedlist::insertLast(int elem){
	Node* to_insert = new Node;
	to_insert->elem = elem;
	size++;
	printf("%s %i\n", "Size is " , getSize());
	if(head==nullptr){
		head = to_insert;
		tail = to_insert;
	}else{
		Node* temp = tail;
		temp->next = to_insert;
		tail = to_insert;
	}
}


void linkedlist::insertAt(int index, int elem){
	Node* to_insert = new Node;
	to_insert->elem = elem;
	if(!(index>size || index<0)){ //index is inbounds
		int curr = 0;
		Node* current = head;
		Node* temp;
		while(curr!=index-1){
			current = current->next;
			curr++;
		}
		temp = current->next;
		current->next = to_insert;
		to_insert->next = temp;
		if(size-1==index){
			tail = to_insert;
		}
		size++;
	}else if(index==0){
		to_insert->next = head;
		head = to_insert;
		size++;
	}else{ //not in bounds
		printf("%s\n", "Not in bounds!");
	}
}


int linkedlist::removeLast(){
	if(head!=nullptr){
		if(head==tail){
			int the_return = head->elem;
			delete head;
			size--;
			return the_return;
		}else{
			Node* current = head;
			for(int x=0;x<size-2;x++){
				current = current->next;
			}
			int the_return = current->next->elem;
			delete current->next;
			tail = current;
			size--;
			return the_return;
		}
	}else{
		return -1;
	}
}


int linkedlist::removeAt(int index){
	if(head!=nullptr){
		if(size-1==index){
			return removeLast();
		}else if(index==0){
			Node* temp = head->next;
			int the_return = head->elem;
			delete head;
			head = temp;
			size--;
			return the_return;
		}else if(!(index>size || index<0)){ //index is inbounds
			int curr = 0;
			Node* current = head;
			Node* temp;
			while(curr!=index){
				current = current->next;
			}
			temp = current->next;
			current->next = temp->next;
			int the_return = temp->elem;
			delete temp;
			size--;
			return the_return;
		}else{ //not in bounds
			printf("%s\n", "Not in bounds!");
			return -1;
		}
	}else{
		return -1;
	}
}


int linkedlist::getSize(){
	return size;
}


bool linkedlist::findElement(int elem){
	Node* current = head;
	for(int x=0;x<size;x++){
		if(current->elem == elem){
			return true;
		}else{
			current = current->next;
		}
	}

	return false; //will be called if for loop is left
}

void linkedlist::printElements(){
	Node* current = head;
	for(int x=0;x<size;x++){
		printf("%i\n", current->elem);
		current = current->next;
	}
}




