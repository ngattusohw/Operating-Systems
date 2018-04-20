/* some tests illustrating the use of thestack.c */

#include <stdlib.h>
#include <stdio.h>

#include "linked_list.h"

int main() {
    LList test_list = makeList();

    //testing if inserting a node into an empty list works
    insertNode(test_list, 1);
    printList(test_list);

    return 0; 
}
