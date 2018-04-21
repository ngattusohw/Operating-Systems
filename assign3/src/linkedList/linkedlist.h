#ifndef LINKED_LIST
#define LINKED_LIST

class linkedlist
{
public:
	linkedlist();
	~linkedlist();
	void insertLast(int elem);
	void insertAt(int index, int elem);
	int removeLast();
	int removeAt(int index);
	int getSize();
	bool findElement(int elem);
	void printElements();
private:
	int size;
	class Node
	{
	public:
		Node();
		~Node();
		Node* next;
		int elem;
	};
	Node* head;
	Node* tail;
};








#endif

