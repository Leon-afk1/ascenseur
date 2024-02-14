#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Structure to represent a User
typedef struct {
    int floor_call;           // Floor where the user called the elevator
    int destination_floor;    // Floor to which the user wants to go
} User;

// Node structure for the linked list of Users
typedef struct UserNode {
    User* user;               // Pointer to the User structure
    struct UserNode* next;    // Pointer to the next UserNode in the list
} UserNode;

// Linked list of Users
typedef struct {
    UserNode* head;    // Pointer to the first UserNode in the list
    int size;          // Number of elements in the list
} UserList;

// Function to create a new UserNode
UserNode* createUser(User* user) {
    UserNode* newUser = (UserNode*)malloc(sizeof(UserNode));
    newUser->user = user;
    newUser->next = NULL;
    return newUser;
}

// Function to delete the head of the list
void deleteHead(UserList* list) {
    if (list->head == NULL) {
        return;
    }
    UserNode* newHead = list->head->next;
    free(list->head);
    list->head = newHead;
    list->size--;
}

// Function to delete a specific User from the list
void deleteUser(UserList* list, User* user) {
    // Special case: if the list is empty
    if (list->head == NULL)
        return;

    // Special case: if the value to delete is in the first node
    if (list->head->user == user) {
        UserNode* temp = list->head;
        list->head = list->head->next;
        free(temp);
        list->size--;
        return;
    }

    // Traverse the list to find the node containing the value to delete
    UserNode* current = list->head;
    UserNode* prev = NULL;
    while (current != NULL && current->user != user) {
        prev = current;
        current = current->next;
    }

    // If the value is not present in the list
    if (current == NULL)
        return;

    // Delete the node containing the value
    prev->next = current->next;
    free(current);
    list->size--;
}

// Function to add a User at the head of the list
void addToHead(UserList* list, User* newUser) {
    UserNode* node = createUser(newUser);
    node->next = list->head;
    list->head = node;
    list->size++;
}

// Function to add an element at the end of the list
void addToEnd(UserList* list, User* newUser) {
    UserNode* node = createUser(newUser);
    if (list->head == NULL) {
        // If the list is empty, the new element becomes the head of the list
        list->head = node;
    } else {
        // Otherwise, traverse the list to the end and add the new element at the end
        UserNode* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }
    list->size++;
}

// Function to traverse the list and print the elements
void printList(UserList* list) {
    if (list->size == 0)
        printf("empty list");
    UserNode* current = list->head;
    while (current != NULL) {
        printf("Call: %d , Destination: %d || ", current->user->floor_call, current->user->destination_floor);
        current = current->next;
    }
    printf("\n");
}

// Function to return the element at the head of the list
User* getHeadElement(UserList* list) {
    if (list->size == 0) {
        return NULL;
    }
    return list->head->user;
}

// Function to return the element at the end of the list
User* getEndElement(UserList* list) {
    UserNode* current = list->head;
    while (current != NULL && current->next != NULL) {
        current = current->next;
    }
    return current->user;
}

// Function to free the memory used by the list
void freeList(UserList* list) {
    UserNode* current = list->head;
    UserNode* next = NULL;
    while (current != NULL) {
        next = current->next;
        free(current->user);
        free(current);
        current = next;
    }
}

// Function to add a User to the list in ascending order of destination floor
void addToAscendingDestination(UserList* list, User* newUser) {
    UserNode* node = createUser(newUser);
    UserNode* current = list->head;
    UserNode* previous = NULL;
    while (current != NULL && current->user->destination_floor < newUser->destination_floor) {
        previous = current;
        current = current->next;
    }
    if (previous == NULL) {
        node->next = list->head;
        list->head = node;
    } else {
        previous->next = node;
        node->next = current;
    }
    list->size++;
}

// Function to add a User to the list in descending order of destination floor
void addToDescendingDestination(UserList* list, User* newUser) {
    UserNode* node = createUser(newUser);
    UserNode* current = list->head;
    UserNode* previous = NULL;
    while (current != NULL && current->user->destination_floor > newUser->destination_floor) {
        previous = current;
        current = current->next;
    }
    if (previous == NULL) {
        node->next = list->head;
        list->head = node;
    } else {
        previous->next = node;
        node->next = current;
    }
    list->size++;
}

// Function to add a User to the list in ascending order of floor call
void addToAscendingCall(UserList* list, User* newUser) {
    UserNode* node = createUser(newUser);
    UserNode* current = list->head;
    UserNode* previous = NULL;
    while (current != NULL && current->user->floor_call < newUser->floor_call) {
        previous = current;
        current = current->next;
    }
    if (previous == NULL) {
        node->next = list->head;
        list->head = node;
    } else {
        previous->next = node;
        node->next = current;
    }
    list->size++;
}

// Function to add a User to the list in descending order of floor call
void addToDescendingCall(UserList* list, User* newUser) {
    UserNode* node = createUser(newUser);
    UserNode* current = list->head;
    UserNode* previous = NULL;
    while (current != NULL && current->user->floor_call > newUser->floor_call) {
        previous = current;
        current = current->next;
    }
    if (previous == NULL) {
        node->next = list->head;
        list->head = node;
    } else {
        previous->next = node;
        node->next = current;
    }
    list->size++;
}
