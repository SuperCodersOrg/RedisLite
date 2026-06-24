#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstdlib>
#include <new>

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
    LinkedList() {
        head = nullptr;
        size = 0;
    }

    LinkedList(const LinkedList& other) {
        head = nullptr;
        size = 0;
        
        SNode<T>* current = other.head;
        while (current) {
            append(current->data);
            current = current->next;
        }
    }

    ~LinkedList() {
        while (head) {
            SNode<T>* temp = head;
            head = head->next;
            temp->data.~T();
            free(temp);
        }
        head = nullptr;
        size = 0;
    }

    void append(T val) {
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

    T get(int index) const {
        if (index < 0 || index >= size) {
            throw "Index out of bounds";
        }
        SNode<T>* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    void insert(int index, T val) {
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

    void remove(int index) {
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

    int getSize() const {
        return size;
    }
};

#endif // LINKED_LIST_H
