#pragma once
#ifndef LINKED_LIST_H
#define LINKED_LIST_H

template<typename T>
struct SNode {
    T data;
    SNode* next;
};

template<typename T>
class LinkedList {
private:
    SNode<T>* head;
    int size;

public:
    LinkedList();
    LinkedList(const LinkedList& other);
    ~LinkedList();

    void append(T val);
    T get(int index) const;
    void insert(int index, T val);
    void remove(int index);
    int getSize() const;
};

#include "../src/customLibrery/linkedList.cpp"
#endif // LINKED_LIST_H
