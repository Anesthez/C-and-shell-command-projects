#include "friends.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef PORT
    #define PORT 56992
#endif


/*
 * Create a new user with the given name.  Insert it at the tail of the list
 * of users whose head is pointed to by *user_ptr_add.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if a user by this name already exists in this list.
 *   - 2 if the given name cannot fit in the 'name' array
 *       (don't forget about the null terminator).
 */
int create_user(const char *name, User **user_ptr_add) {

    User *new_user = malloc(sizeof(User));
    if (new_user == NULL) {
        perror("malloc");
        exit(1);
    }
    strncpy(new_user->name, name, MAX_NAME); // name has max length MAX_NAME - 1

    for (int i = 0; i < MAX_NAME; i++) {
        new_user->profile_pic[i] = '\0';
    }

    new_user->first_post = NULL;
    new_user->next = NULL;
    for (int i = 0; i < MAX_FRIENDS; i++) {
        new_user->friends[i] = NULL;
    }

    // Add user to list
    User *prev = NULL;
    User *curr = *user_ptr_add;
    while (curr != NULL && strcmp(curr->name, name) != 0) {
        prev = curr;
        curr = curr->next;
    }

    if (*user_ptr_add == NULL) {
        *user_ptr_add = new_user;
        return 0;
    } else if (curr != NULL) {
        free(new_user);
        return 1;
    } else {
        prev->next = new_user;
        return 0;
    }
}


/*
 * Return a pointer to the user with this name in
 * the list starting with head. Return NULL if no such user exists.
 *
 * NOTE: You'll likely need to cast a (const User *) to a (User *)
 * to satisfy the prototype without warnings.
 */
User *find_user(const char *name, const User *head) {
    while (head != NULL && strcmp(name, head->name) != 0) {
        head = head->next;
    }

    return (User *)head;
}


/*
 * Print the usernames of all users in the list starting at curr.
 * Names should be printed to standard output, one per line.
 */
char *list_users(const User *curr) {
    //set up the initial count
    int size = 0;
    size += 13;

    User *cal_curr = (User *)curr;

    while (cal_curr != NULL) {
        size += 3 + strlen(cal_curr->name);
        cal_curr = cal_curr->next;
        //loop through the linked list to calculate the size of string
    }

    char *buffer = malloc(size);
    //create buffer according to size

    if(buffer == NULL){
        perror("malloc");
        exit(1);
    }
    //error check for malloc

    char *after = buffer;
    buffer[0] = '\0';
    int room = size - 1;
    int r;
    //set up flags

    r = snprintf(after, room, "User List\r\n");
    //print the message with internet new line to the list
    room = room - r + 1;
    after = buffer + r;
    //update places

    while (curr != NULL) {
        r = snprintf(after, room, "\t%s\r\n",curr->name);
        room = room - r + 1;
        after = after + r;
        curr = curr->next;
    }


    return buffer;
}


/*
 * Make two users friends with each other.  This is symmetric - a pointer to
 * each user must be stored in the 'friends' array of the other.
 *
 * New friends must be added in the first empty spot in the 'friends' array.
 *
 * Return:
 *   - 0 on success.
 *   - 1 if the two users are already friends.
 *   - 2 if the users are not already friends, but at least one already has
 *     MAX_FRIENDS friends.
 *   - 3 if the same user is passed in twice.
 *   - 4 if at least one user does not exist.
 *
 * Do not modify either user if the result is a failure.
 * NOTE: If multiple errors apply, return the *largest* error code that applies.
 */
int make_friends(const char *name1, const char *name2, User *head) {
    User *user1 = find_user(name1, head);
    User *user2 = find_user(name2, head);

    if (user1 == NULL || user2 == NULL) {
        return 4;
    } else if (user1 == user2) { // Same user
        return 3;
    }

    int i, j;
    for (i = 0; i < MAX_FRIENDS; i++) {
        if (user1->friends[i] == NULL) { // Empty spot
            break;
        } else if (user1->friends[i] == user2) { // Already friends.
            return 1;
        }
    }

    for (j = 0; j < MAX_FRIENDS; j++) {
        if (user2->friends[j] == NULL) { // Empty spot
            break;
        }
    }

    if (i == MAX_FRIENDS || j == MAX_FRIENDS) { // Too many friends.
        return 2;
    }

    user1->friends[i] = user2;
    user2->friends[j] = user1;
    return 0;
}


/*
 *  Print a post.
 *  Use localtime to print the time and date.
 */
char *print_post(const Post *post) {
    int size = 0;

    size += (2 + 8 + 8 + 2 + strlen(post -> author) + 
    strlen(asctime(localtime(post->date))) + 
    strlen(post->contents));

    //adding up the size of the strings
    
    char *buffer = malloc(size);
    
    if(buffer == NULL){
        perror("malloc");
        exit(1);
    }//error check for malloc

    char *after = buffer;
    buffer[0] = '\0';
    int room = size - 1;
    int r;

    
    if (post == NULL) {
        return buffer;
    }
    // Print author
    r = snprintf(after, room, "From: %s\r\n", post->author);
    after = after + r;
    room = room - r + 1;

    // Print date
    r = snprintf(after, room, "Date: %s\r\n", asctime(localtime(post->date)));
    after = after + r;
    room = room - r + 1;


    // Print message
    r = snprintf(after, room, "%s\r\n", post->contents);
    after = after + r;
    room = room - r + 1;


    return buffer;
}


/*
 * Print a user profile.
 * For an example of the required output format, see the example output
 * linked from the handout.
 * Return:
 *   - 0 on success.
 *   - 1 if the user is NULL.
 */
char *print_user(const User *user) {
    int size = 0;

    size += (10 + strlen(user -> name) + 44 + 10 + 44);
    //setting the size for peripheral values

    for (int i = 0; i < MAX_FRIENDS && user->friends[i] != NULL; i++) {
        size += (2 + strlen(user->friends[i]->name));
        //loop through the list to calculate the space needed
    }

    size += 8;

    const Post *val_curr = user->first_post;
    while (val_curr != NULL) {
        char *post_buf = print_post(val_curr);
        size += (strlen(post_buf));
        val_curr = val_curr->next;
        free(post_buf);
        if (val_curr != NULL) {
            size += 9;
        }
        //adding size to the cumulator
    }
    size += 44;

    char *buffer = malloc(size);

    if(buffer == NULL){
        perror("malloc");
        exit(1);
    }//error checking for malloc

    char *after = buffer;
    buffer[0] = '\0';
    int room = size - 1;
    int r;

    if (user == NULL) {
        return buffer;
    }



    // Print name
    r = snprintf(after, room, "Name: %s\r\n", user->name);
    after = after + r;
    room = room - r + 1;
    r = snprintf(after, room, "------------------------------------------\r\n");
    after = after + r;
    room = room - r + 1;

    // Print friend list.
    r = snprintf(after, room, "Friends:\r\n");
    after = after + r;
    room = room - r + 1;

    for (int i = 0; i < MAX_FRIENDS && user->friends[i] != NULL; i++) {
        r = snprintf(after, room, "%s\r\n", user->friends[i]->name);
        after = after + r;
        room = room - r + 1;
    }
    r = snprintf(after, room, "------------------------------------------\r\n");
    after = after + r;
    room = room - r + 1;

    // Print post list.
    r = snprintf(after, room, "Posts:\r\n");
    after = after + r;
    room = room - r + 1;

    const Post *curr = user->first_post;
    while (curr != NULL) {
        char *p_buf = print_post(curr);
        r = snprintf(after, room, "%s", p_buf);
        after = after + r;
        room = room - r + 1;
        curr = curr->next;
        free(p_buf);
        if (curr != NULL) {
            r = snprintf(after, room, "\r\n===\r\n\r\n");
            after = after + r;
            room = room - r + 1;
        }
    }

    r = snprintf(after, room, "------------------------------------------\r\n");
    after = after + r;
    room = room - r + 1;

    buffer[strlen(buffer) - 1] = '\r';
    buffer[strlen(buffer)] = '\n';
    buffer[strlen(buffer) + 1] = '\0';

    return buffer;
}


/*
 * Make a new post from 'author' to the 'target' user,
 * containing the given contents, IF the users are friends.
 *
 * Insert the new post at the *front* of the user's list of posts.
 *
 * Use the 'time' function to store the current time.
 *
 * 'contents' is a pointer to heap-allocated memory - you do not need
 * to allocate more memory to store the contents of the post.
 *
 * Return:
 *   - 0 on success
 *   - 1 if users exist but are not friends
 *   - 2 if either User pointer is NULL
 */
int make_post(const User *author, User *target, char *contents) {
    if (target == NULL || author == NULL) {
        return 2;
    }

    int friends = 0;
    for (int i = 0; i < MAX_FRIENDS && target->friends[i] != NULL; i++) {
        if (strcmp(target->friends[i]->name, author->name) == 0) {
            friends = 1;
            break;
        }
    }

    if (friends == 0) {
        return 1;
    }

    // Create post
    Post *new_post = malloc(sizeof(Post));
    if (new_post == NULL) {
        perror("malloc");
        exit(1);
    }
    strncpy(new_post->author, author->name, MAX_NAME);
    new_post->contents = contents;
    new_post->date = malloc(sizeof(time_t));
    if (new_post->date == NULL) {
        perror("malloc");
        exit(1);
    }
    time(new_post->date);
    new_post->next = target->first_post;
    target->first_post = new_post;

    return 0;
}

