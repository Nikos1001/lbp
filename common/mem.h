
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
