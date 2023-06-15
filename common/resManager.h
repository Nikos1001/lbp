
#ifndef RES_MANAGER_H
#define RES_MANAGER_H

#include "mem.h"

template<typename T>
class ResManager {

public:

    static ResManager<T>* get() {
        if(!instance) {
            instance = (ResManager<T>*)heapAlloc(sizeof(ResManager<T>));
            instance->res.init();
        }
        return instance;
    }

    ResManager() {}

    Arr<T, true> res;
    static inline ResManager<T>* instance;

};

template<typename T>
class ResHandle {

public:

    T* get() {
        return &ResManager<T>::get()->res[idx];
    }

    T* operator->() {
        return get();
    }

    uint16_t idx;

};

template<typename T>
ResHandle<T> addResource(T res) {
    ResManager<T>* manager = ResManager<T>::get();
    ResHandle<T> handle;
    handle.idx = manager->res.cnt();
    manager->res.add(res);
    return handle;
}

#endif
