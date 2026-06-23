#include <cstdlib>
#include <new>

template<typename T>
struct Node {
    T data;
    Node* next;
};

template<typename T>
class LinkedList {
private:
    Node<T>* head;
    int size;

public:
    LinkedList() {
        head = nullptr;
        size = 0;
    }

    LinkedList(const LinkedList& other) {
        head = nullptr;
        size = 0;
        
        Node<T>* current = other.head;
        while (current) {
            append(current->data);
            current = current->next;
        }
    }

    ~LinkedList() {
        while (head) {
            Node<T>* temp = head;
            head = head->next;
            temp->data.~T();
            free(temp);
        }
        head = nullptr;
        size = 0;
    }

    void append(T val) {
        Node<T>* newNode = (Node<T>*)malloc(sizeof(Node<T>));
        new (&newNode->data) T(val);
        newNode->next = nullptr;

        if (!head) {
            head = newNode;
        } else {
            Node<T>* current = head;
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
        Node<T>* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    void insert(int index, T val) {
        if (index < 0 || index > size) {
            throw "Index out of bounds";
        }

       
        Node<T>* newNode = (Node<T>*)malloc(sizeof(Node<T>));
        new (&newNode->data) T(val);

        if (index == 0) {
            newNode->next = head;
            head = newNode;
        } else {
            Node<T>* current = head;
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

        Node<T>* temp = nullptr;
        if (index == 0) {
            temp = head;
            head = head->next;
        } else {
            Node<T>* current = head;
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
