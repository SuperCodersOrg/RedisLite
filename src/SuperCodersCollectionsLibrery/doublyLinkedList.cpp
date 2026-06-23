#include <cstdlib>
#include <new>

template<typename T>
struct Node {
    T data;
    Node* next;
    Node* prev;
};

template<typename T>
class DoublyLinkedList {
private:
    Node<T>* head;
    Node<T>* tail;
    int size;

public:
    DoublyLinkedList() {
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

        
    DoublyLinkedList(const DoublyLinkedList& other) {
        head = nullptr;
        tail = nullptr;
        size = 0;
        
        Node<T>* current = other.head;
        while (current) {
            append(current->data);
            current = current->next;
        }
    }

    ~DoublyLinkedList() {
        while (head) {
            Node<T>* temp = head;
            head = head->next; 
            temp->data.~T();
            free(temp);
        }
        head = nullptr;
        tail = nullptr;
        size = 0;
    }

        
    void append(T val) {
        Node<T>* newNode = (Node<T>*)malloc(sizeof(Node<T>));
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

    
    T get(int index) const {
        if (index < 0 || index >= size) {
            throw "Index out of bounds";
        }
        
        Node<T>* current;
        
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

    
    void insert(int index, T val) {
        if (index < 0 || index > size) {
            throw "Index out of bounds";
        }
        if (index == size) {
            append(val);
            return;
        }

        Node<T>* newNode = (Node<T>*)malloc(sizeof(Node<T>));
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
            Node<T>* current = head;
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

    void remove(int index) {
        if (index < 0 || index >= size) {
            throw "Index out of bounds";
        }

        Node<T>* temp = nullptr;
        
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
            Node<T>* current = head;
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

    int getSize() const {
        return size;
    }
};
