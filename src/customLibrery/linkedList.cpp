#pragma once
#ifndef LINKED_LIST_CPP
#define LINKED_LIST_CPP

#include "linkedList.h"
#include <cstdlib>
#include <new>

template<typename T>
LinkedList<T>::LinkedList() {
    head = nullptr;
    size = 0;
}

template<typename T>
LinkedList<T>::LinkedList(const LinkedList& other) {
    head = nullptr;
    size = 0;
    
    SNode<T>* current = other.head;
    while (current) {
        append(current->data);
        current = current->next;
    }
}

template<typename T>
LinkedList<T>::~LinkedList() {
    while (head) {
        SNode<T>* temp = head;
        head = head->next;
        temp->data.~T();
        free(temp);
    }
    head = nullptr;
    size = 0;
}

template<typename T>
void LinkedList<T>::append(T val) {
    SNode<T>* newNode = (SNode<T>*)malloc(sizeof(SNode<T>));
    new (&newNode->data) T(val);
    newNode->next = nullptr;

    if (!head) {
        head = newNode;
    } else {
        SNode<T>* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }
    size++;
}

template<typename T>
T LinkedList<T>::get(int index) const {
    if (index < 0 || index >= size) {
        throw "Index out of bounds";
    }
    SNode<T>* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->data;
}

template<typename T>
void LinkedList<T>::insert(int index, T val) {
    if (index < 0 || index > size) {
        throw "Index out of bounds";
    }

    SNode<T>* newNode = (SNode<T>*)malloc(sizeof(SNode<T>));
    new (&newNode->data) T(val);

    if (index == 0) {
        newNode->next = head;
        head = newNode;
    } else {
        SNode<T>* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
    size++;
}

template<typename T>
void LinkedList<T>::remove(int index) {
    if (index < 0 || index >= size) {
        throw "Index out of bounds";
    }

    SNode<T>* temp = nullptr;
    if (index == 0) {
        temp = head;
        head = head->next;
    } else {
        SNode<T>* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        temp = current->next;
        current->next = temp->next;
    }
    temp->data.~T();
    free(temp);
    size--;
}

template<typename T>
int LinkedList<T>::getSize() const {
    return size;
}

#endif // LINKED_LIST_CPP
