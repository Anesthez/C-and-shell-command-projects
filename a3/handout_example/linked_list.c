/* Linked list implementation, based on this PCRS module:
 * https://pcrs.teach.cs.toronto.edu/csc209-2023-01/content/challenges/50/1
 *
 * Note that this file does not have a "main" function.
 */
#include <stdio.h>
#include <stdlib.h>

#include "linked_list.h"


/* Create and return a new Node with the supplied values. */
Node *create_node(int num, Node *next) {
    Node *new_node = malloc(sizeof(Node));
    new_node->value = num;
    new_node->next = next;
    return new_node;
}


/* Insert a new node with the value num into this position of list front.
   Return 0 on success and -1 on failure. */
int insert(int num, Node **front_ptr, int position) {
    Node *curr = *front_ptr;

    if (position == 0) {
        *front_ptr = create_node(num, *front_ptr);
        return 0;
    }

    for (int i = 0; i < position - 1 && curr != NULL; i++) {
        curr = curr->next;
    }
    if (curr == NULL) {
        return -1;
    }
    Node *new_node = create_node(num, curr->next);
    curr->next = new_node;

    return 0;
}


/* Remove the head of the linked list. Return the item stored in the head,
 * or -1 if the linked list was empty.
 */
int delete_head(Node **front_ptr) {
    Node *head = *front_ptr;
    if (head == NULL) {
        return -1;
    } else {
        int val = head->value;
        *front_ptr = head->next;
        free(head);
        return val;
    }
}


/* Print all items in the given linked list. */
void print_list(Node **front_ptr) {
    Node *curr = *front_ptr;
    while (curr != NULL) {
        printf("%d ", curr->value);
        curr = curr->next;
    }
    printf("\n");
}
