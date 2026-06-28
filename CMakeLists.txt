cmake_minimum_required(VERSION 3.10)
project(RedisLite)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(CustomLibrary
    src/customLibrery/dynamicArray.cpp
    src/customLibrery/doublyLinkedList.cpp
    src/customLibrery/linkedList.cpp
    src/customLibrery/hashFunction.cpp
    src/customLibrery/hashMap.cpp
)
target_include_directories(CustomLibrary PUBLIC include)

add_executable(RedisLite main.cpp)
target_link_libraries(RedisLite CustomLibrary)
