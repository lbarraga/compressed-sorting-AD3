//
// Created by lukasbt on 10/19/23.
//

#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

Node* initLinkedList(int initialData) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->data = initialData;
    newNode->next = NULL;
    return newNode;
}

Node* initEmptyLinkedList() {
    return NULL; // Returns a NULL pointer, indicating an empty list.
}


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


void addList(Node** destList, Node* srcList) {
    while (srcList != NULL) {
        append(destList, srcList->data);
        srcList = srcList->next;
    }
}

void deleteAtIndex(Node** headRef, int index) {
    if (*headRef == NULL) {
        return;
    }

    Node* temp = *headRef;

    if (index == 0) {
        *headRef = temp->next;
        free(temp);
        return;
    }

    int i = 0;
    while (i < index - 1 && temp) {
        temp = temp->next;
        ++i;
    }

    if (temp == NULL || temp->next == NULL) {
        return;
    }

    Node* nextNode = temp->next->next;

    free(temp->next);
    temp->next = nextNode;
}

int getLength(Node* head) {
    int length = 0;
    Node* current = head;

    while (current != NULL) {
        length++;
        current = current->next;
    }

    return length;
}

int* toArray(Node* head) {
    int length = getLength(head);
    int* array = malloc(sizeof(int) * length);

    Node* current = head;
    for (int i = 0; i < length; i++) {
        array[i] = current->data;
        current = current->next;
    }

    return array;
}


void freeLinkedList(Node* head) {
    Node* tmp;

    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}
