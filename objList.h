
#ifndef OBJLIST_H
#define OBJLIST_H

template<typename T>
struct ObjListNode {
    T obj; // MUST ALWAYS BE FIRST IN STRUCT
    ObjListNode<T>* next;
    bool active;
};

template<typename T>
class ObjList {

public:

    static void init() {
        free = NULL;
        cnt = 0;
    }

    static T* spawn() {
        ObjListNode<T>* result;
        if(free) {
            result = free;
            free = free->next;
        } else {
            result = &objs[cnt];
            cnt++;
        }
        result->active = true;
        return &result->obj;
    }

    static void kill(T* obj) {
        obj->free();
        ObjListNode<T>* node = (ObjListNode<T>*)obj; 
        node->next = free;
        node->active = false;
        free = node; 
    }

    static T* first() {
        for(int i = 0; i < cnt; i++)
            if(objs[i].active)
                return &objs[i].obj;
        return NULL;
    }

    static T* next(T* curr) {
        int i = (ObjListNode<T>*)curr - objs;
        i++;
        for(; i < cnt; i++)
            if(objs[i].active)
                return &objs[i].obj;
        return NULL;
    }

private:

    // silly way to allocate things. relies on os only allocating globals that are used
    // but this guarantees that things don't move around in memory, which is pretty cool
    static inline ObjListNode<T> objs[UINT16_MAX];
    static inline ObjListNode<T>* free;
    static inline int cnt;

};

#endif
