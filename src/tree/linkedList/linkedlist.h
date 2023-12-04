//
// Created by lukasbt on 10/19/23.
//

#ifndef PROJECTAD3_LINKEDLIST_H
#define PROJECTAD3_LINKEDLIST_H

// Node structure for singly linked list
typedef struct Node {
    int data;
    struct Node* next;
} Node;


Node* initLinkedList(int initialData);
Node* initEmptyLinkedList();
void append(Node** headRef, int data);
void addList(Node** destList, Node* srcList);
void freeLinkedList(Node* head);
int* toArray(Node* head);
void deleteAtIndex(Node** headRef, int index);
int getLength(Node* head);

#endif //PROJECTAD3_LINKEDLIST_H
