/**
 * Header file for a small linked list library.
 */
typedef struct node {
    int value;
    struct node *next;
} Node;

Node *create_node(int num, Node *next);
int insert(int num, Node **front_ptr, int position);
int delete_head(Node **front_ptr);
void print_list(Node **front_ptr);
