//
// Created by lukasbt on 10/19/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

// Initialize a linked list with one element and return its head
Node* initLinkedList(int initialData) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = initialData;
    newNode->next = NULL;
    return newNode;
}

// Add an element to the end of the linked list
void append(Node** headRef, int data) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (*headRef == NULL) {
        *headRef = newNode;
        return;
    }

    Node* lastNode = *headRef;
    while (lastNode->next != NULL) {
        lastNode = lastNode->next;
    }
    lastNode->next = newNode;
}

// Add all elements from srcList to destList
void addList(Node** destList, Node* srcList) {
    while (srcList != NULL) {
        append(destList, srcList->data);
        srcList = srcList->next;
    }
}

// Delete the node at the given index
void deleteAtIndex(Node** headRef, int index) {
    if (*headRef == NULL) {
        return;
    }

    Node* temp = *headRef;

    // If head needs to be removed
    if (index == 0) {
        *headRef = temp->next;
        free(temp);
        return;
    }

    // Find the node before the node to be deleted
    int i = 0;
    while (i < index - 1 && temp) {
        temp = temp->next;
        ++i;
    }

    // If the index is out of bounds
    if (temp == NULL || temp->next == NULL) {
        return;
    }

    // Node temp->next is the node to be deleted
    // Store pointer to the next of node to be deleted
    Node* nextNode = temp->next->next;

    // Unlink the node from the linked list and free it
    free(temp->next);
    temp->next = nextNode;
}



// Free the memory allocated for the linked list
void freeLinkedList(Node* head) {
    Node* tmp;

    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}
