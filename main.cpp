#include <iostream>
#include <string>
#include "SuperCodersCollections.h"

int main() {
    std::cout << "--- Testing Custom Data Structures ---\n\n";

    // 1. Testing DynamicArray
    std::cout << "[1] Testing DynamicArray:\n";
    DynamicArray<int> arr;
    arr.append(10);
    arr.append(20);
    arr.append(30);
    arr.insert(1, 15); // should be 10, 15, 20, 30
    arr.remove(2);     // removes 20 -> 10, 15, 30
    
    std::cout << "DynamicArray elements: ";
    for (int i = 0; i < arr.getSize(); i++) {
        std::cout << arr.get(i) << " ";
    }
    std::cout << "\n\n";

    // 2. Testing LinkedList
    std::cout << "[2] Testing LinkedList:\n";
    LinkedList<std::string> list;
    list.append("Hello");
    list.append("World");
    list.insert(1, "Beautiful"); // Hello Beautiful World
    
    std::cout << "LinkedList elements: ";
    for (int i = 0; i < list.getSize(); i++) {
        std::cout << list.get(i) << " ";
    }
    std::cout << "\n\n";

    // 3. Testing DoublyLinkedList
    std::cout << "[3] Testing DoublyLinkedList:\n";
    DoublyLinkedList<double> dlist;
    dlist.append(1.1);
    dlist.append(2.2);
    dlist.append(3.3);
    dlist.remove(0); // removes 1.1 -> 2.2, 3.3
    
    std::cout << "DoublyLinkedList elements: ";
    for (int i = 0; i < dlist.getSize(); i++) {
        std::cout << dlist.get(i) << " ";
    }
    std::cout << "\n\n";

    // 4. Testing HashMap
    std::cout << "[4] Testing HashMap:\n";
    HashMap<std::string, int> map;
    map.set("Alice", 25);
    map.set("Bob", 30);
    map.set("Charlie", 35);
    map.set("Alice", 26); // Update Alice

    std::cout << "HashMap Alice's age: " << map.get("Alice") << "\n";
    std::cout << "HashMap Bob's age: " << map.get("Bob") << "\n";
    std::cout << "HashMap Size: " << map.getSize() << "\n\n";

    std::cout << "All custom data structures tested successfully!\n";

    return 0;
}
