#pragma once
#ifndef DOUBLY_LINKED_LIST_CPP
#define DOUBLY_LINKED_LIST_CPP

#include "doublyLinkedList.h"
#include <cstdlib>
#include <new>

template<typename T>
DoublyLinkedList<T>::DoublyLinkedList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

template<typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList& other) {
    head = nullptr;
    tail = nullptr;
    size = 0;
    
    DNode<T>* current = other.head;
    while (current) {
        append(current->data);
        current = current->next;
    }
}

template<typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList& other) {
    if (this == &other) return *this;
    
    while (head) {
        DNode<T>* temp = head;
        head = head->next;
        temp->data.~T();
        free(temp);
    }
    
    head = nullptr;
    tail = nullptr;
    size = 0;
    
    DNode<T>* current = other.head;
    while (current) {
        append(current->data);
        current = current->next;
    }
    return *this;
}

template<typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    while (head) {
        DNode<T>* temp = head;
        head = head->next; 
        temp->data.~T();
        free(temp);
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

template<typename T>
void DoublyLinkedList<T>::append(T val) {
    DNode<T>* newNode = (DNode<T>*)malloc(sizeof(DNode<T>));
    new (&newNode->data) T(val);
    newNode->next = nullptr;
    newNode->prev = nullptr;

    if (!head) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }
    size++;
}

template<typename T>
T DoublyLinkedList<T>::get(int index) const {
    if (index < 0 || index >= size) {
        throw "Index out of bounds";
    }
    
    DNode<T>* current;
    
    if (index < size / 2) {
        current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
    } else {
        current = tail;
        for (int i = size - 1; i > index; i--) {
            current = current->prev;
        }
    }
    return current->data;
}

template<typename T>
void DoublyLinkedList<T>::insert(int index, T val) {
    if (index < 0 || index > size) {
        throw "Index out of bounds";
    }
    if (index == size) {
        append(val);
        return;
    }

    DNode<T>* newNode = (DNode<T>*)malloc(sizeof(DNode<T>));
    new (&newNode->data) T(val);

    if (index == 0) {
        newNode->next = head;
        newNode->prev = nullptr;
        if (head) {
            head->prev = newNode;
        }
        head = newNode;
        if (!tail) {
            tail = newNode; 
        }
    } else {
        DNode<T>* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        
        newNode->next = current;
        newNode->prev = current->prev;
        
        current->prev->next = newNode;
        current->prev = newNode;
    }
    size++;
}

template<typename T>
void DoublyLinkedList<T>::remove(int index) {
    if (index < 0 || index >= size) {
        throw "Index out of bounds";
    }

    DNode<T>* temp = nullptr;
    
    if (index == 0) {
        temp = head;
        head = head->next;
        if (head) {
            head->prev = nullptr;
        } else { 
            tail = nullptr;
        }
    } else if (index == size - 1) {
        temp = tail;
        tail = tail->prev;
        if (tail) {
            tail->next = nullptr;
        } else { 
            head = nullptr;
        }
    } else {
        DNode<T>* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        temp = current;
        current->prev->next = current->next;
        current->next->prev = current->prev;
    }

    temp->data.~T();
    free(temp);
    size--;
}

template<typename T>
int DoublyLinkedList<T>::getSize() const {
    return size;
}

#endif // DOUBLY_LINKED_LIST_CPP
