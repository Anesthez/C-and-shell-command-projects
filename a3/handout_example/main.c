/**
 * This is the main program.
 * It needs to use the code defined in linked_list.c
*/
#include <stdlib.h>

// Declarations from linked_list.c
#include "linked_list.h"

int main() {
    // Linked list version
    Node *head = NULL;
    insert(10, &head, 0);
    insert(20, &head, 0);
    insert(30, &head, 0);
    print_list(&head);

    return 0;
}
