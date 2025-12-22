# Memory pool

The speed at which memory for an object can be allocated and deallocated using
either malloc/free or new/delete can for some use cases pose a problem. General
purpose allocators typically have to:

- Handle arbitrary allocation sizes, in any order
- Minimize memory fragmentation
- Be thread safe
- Make the best use of operating system features such as virtual memory

A common technique to improve allocation speed is to use a memory pool.

# Exercise

Implement a memory pool in src/pool.hpp that fulfills following requirements:

- All object in the pool are of the same type.
- The maximum number of objects is known when the pool is created.
- Allocation and destruction of objects should run the constructor and destructor.

There are unit tests available that can be built and run like this:

```
$ cmake -B build
$ cmake --build build
$ ctest --test-dir build
```
