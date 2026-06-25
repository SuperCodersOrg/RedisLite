# Date: June 22 
Duration: 80 minutes 

Goal: 
Implement a generic DynamicArray from scratch (without using any built-in STL containers or libraries).

Problem Encountered: 
1. Accidentally violated project constraints by including `<iostream>`, `using namespace std;`, and `<stdexcept>` to handle out-of-bounds exceptions using `std::out_of_range`.
2. Compilation and variable shadowing issues: The `size()` method conflicted with the `size` member variable, and there were variable mismatches (`data` instead of `arr` in the copy constructor, and a typo `capactiy` in the insert method).
3. Needed to ensure efficient access to array elements without causing expensive memory copies for large objects.
4. Reviewed C++ template memory rules and realized that `new` and `delete` are native C++ language keywords, not STL library components, making them perfectly legal to use without imports.

## error in the code : 
D:\projects\RedisLite>gcc dynamicArray.cpp -o app
cc1plus.exe: fatal error: dynamicArray.cpp: No such file or directory
compilation terminated.

D:\projects\RedisLite>cd SuperCodersCollectionsLibrery

D:\projects\RedisLite\SuperCodersCollectionsLibrery>gcc dynamicArray.cpp -o app
dynamicArray.cpp:72:5: error: 'int DynamicArray<T>::size()' conflicts with a previous declaration [-Wtemplate-body]
   72 |     }
      |     ^
dynamicArray.cpp:5:13: note: previous declaration 'int DynamicArray<T>::size'
    5 |         int size;
      |             ^~~~
dynamicArray.cpp: In constructor 'DynamicArray<T>::DynamicArray( 
':
dynamicArray.cpp:19:23: error: there are no arguments to 'malloc' that depend on a template parameter, so a declaration of 'mallo 
' must be available [-Wtemplate-body]
   19 |             arr = (T*)malloc(capacity*sizeof(T));        
      |                       ^~~~~~
dynamicArray.cpp:19:23: note: (if you use '-fpermissive', G++ will accept your code, but allowing the use of an undeclared name is deprecated)
dynamicArray.cpp: In copy constructor 'DynamicArray<T>::DynamicArray(const DynamicArray<T>&)':
dynamicArray.cpp:26:23: error: there are no arguments to 'malloc' that depend on a template parameter, so a declaration of 'mallo 
' must be available [-Wtemplate-body]
   26 |             arr = (T*)malloc(capacity*sizeof(T));        
      |                       ^~~~~~
dynamicArray.cpp: In member function 'T& DynamicArray<T>::get(int)':
dynamicArray.cpp:44:23: error: there are no arguments to 'out_of_range' that depend on a template parameter, so a declaration of  
out_of_range' must be available [-Wtemplate-body]
   44 |                 throw out_of_range("Index out of bounds");
      |                       ^~~~~~~~~~~~
dynamicArray.cpp: In member function 'void DynamicArray<T>::insert(int, T)':        
dynamicArray.cpp:50:23: error: there are no arguments to 'out_of_range' that depend 
on a template parameter, so a declaration of
out_of_range' must be available [-Wtemplate-body]
   50 |                 throw out_of_range("Index out of bounds");
      |                       ^~~~~~~~~~~~
dynamicArray.cpp: In member function 'void DynamicArray<T>::remove(int)':
dynamicArray.cpp:63:23: error: there are no arguments to 'out_of_range' that depend 
on a template parameter, so a declaration of
out_of_range' must be available [-Wtemplate-body]
   63 |                 throw out_of_range("Index out of bounds");

## What I Tried: 
1. Researched ways to throw errors without relying on the standard library.
2. Manually traced through the class variables to fix typos and resolve the naming conflict in the `size()` method by making it `getSize()`.
3. Implemented a pass-by-reference return type (`T&`) in the `get()` method to avoid deep copies and allow direct modification of the array elements.

## Outcome: 
Successfully removed all standard library imports and without using new and delete keywords. Replaced standard exceptions with a simple string literal `throw "Index out of bounds";` with declaration. Fixed all typos and shadowing issues. The DynamicArray is now fully generic, dynamically resizes, implements the Rule of Three for manual memory management, returns references for O(1) high-performance access, and strictly adheres to the zero-STL project constraint.
