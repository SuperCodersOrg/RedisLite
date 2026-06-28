#pragma once
#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

template<typename T>
struct DNode {
    T data;
    DNode* next;
    DNode* prev;
};

template<typename T>
class DoublyLinkedList {
private:
    DNode<T>* head;
    DNode<T>* tail;
    int size;

public:
    DoublyLinkedList();
    DoublyLinkedList(const DoublyLinkedList& other);
    DoublyLinkedList& operator=(const DoublyLinkedList& other);
    ~DoublyLinkedList();
    
    void append(T val);
    T get(int index) const;
    void insert(int index, T val);
    void remove(int index);
    int getSize() const;
};

#include "../src/customLibrery/doublyLinkedList.cpp"
#endif // DOUBLY_LINKED_LIST_H
