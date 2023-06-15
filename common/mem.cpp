
#include "mem.h"

void* heapAlloc(size_t size) {
    return malloc(size);
}

void heapFree(void* alloc) {
    free(alloc);
}

void* heapRealloc(void* alloc, size_t newSize) {
    return realloc(alloc, newSize);
}

// a *gigabyte* ought to be enough :)
static char arena[(size_t)1024 * 1024 * 1024];
static char* arenaCurr;

void arenaInit() {
    arenaCurr = arena;
}

void* arenaAlloc(size_t size) {
    void* res = (void*)arenaCurr;
    arenaCurr += size;
    return res;
}

void arenaClear() {
    arenaCurr = arena;
}

int calcNewCap(int cap) {
    if(cap == 0)
        return 8;
    return 2 * cap;
}