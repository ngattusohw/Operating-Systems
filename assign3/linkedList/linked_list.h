
/* Simple implementation for a linked list in C

   This was implemented on a plane from LAW to EWR, in about 30 minutes

   @author Nick Gattuso
   @since 3/15/17
*/




struct llist;

typedef struct llist* LList;

LList makeList();

void insertNode(LList the_list, int v);

void removeNode(LList the_list, int v);

void printList(LList the_list);

int getCount(LList the_list);

//LList reverseList(LList the_list);