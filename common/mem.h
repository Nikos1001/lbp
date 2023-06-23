
#ifndef MEM_H
#define MEM_H

#include "util.h"

void* heapAlloc(size_t size);
void heapFree(void* alloc);
void* heapRealloc(void* alloc, size_t newSize);

void arenaInit();
void* arenaAlloc(size_t size);
void arenaClear();

int calcNewCap(int cap);

template<typename T, bool onHeap>
class Arr {

public:
    void init() {
        elems = NULL;
        size = 0;
        cap = 0;
    }

    void free() {
        if(onHeap) {
            heapFree((void*)elems);
        }
    }

    int add(T elem) {
        size++;
        if(size > cap) {
            grow();
        }
        this->elems[size - 1] = elem;
        return size - 1;
    }

    void pop() {
        size--;
    }

    void clear() {
        size = 0;
    }

    void removeAt(int i) {
        memcpy((void*)(elems + i), (void*)(elems + i + 1), sizeof(T) * (size - i - 1));
        size--;
    }

    void insertAt(int i, T elem) {
        size++;
        if(size > cap) {
            grow();
        }
        memcpy((void*)(elems + i + 1), (void*)(elems + i), sizeof(T) * (size - i - 1));
        this->elems[i] = elem;
    }

    T& operator[](int idx) {
        return elems[idx];
    }

    int cnt() {
        return size;
    }


    T* elems;

private:
    int size;
    int cap;
    
    void grow() {
        if(!onHeap) {
            int newCap = calcNewCap(cap);
            T* newElems = (T*)arenaAlloc(newCap * sizeof(T));
            memcpy(newElems, elems, cap * sizeof(T));
            elems = newElems;
            cap = newCap;
        } else {
            int newCap = calcNewCap(cap);
            elems = (T*)heapRealloc(elems, newCap * sizeof(T));
            cap = newCap;
        }
    }

};

#endif
