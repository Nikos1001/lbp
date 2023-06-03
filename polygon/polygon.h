
#ifndef POLYGON_H
#define POLYGON_H

#include "../mem.h"
#include "../util.h"
#include "geoutil.h"

struct Vert {
    glm::vec2 pt;
    int prev;
    int next;
    int chain;
};

template<bool onHeap>
class Polygon {

public:
    void init() {
        verts.init();
        chains.init();
        chainLen.init();
    }

    void free() {
        verts.free();
        chains.free();
        chainLen.free();
    }

    int addChain(glm::vec2 pt) {
        verts.add((Vert){pt, -1, -1, chains.cnt()});
        chains.add(verts.cnt() - 1);
        chainLen.add(1);
        return verts.cnt() - 1;
    }

    int addPoint(glm::vec2 pt, int prev) {
        verts.add((Vert){pt, prev, -1, verts[prev].chain});
        verts[prev].next = verts.cnt() - 1;
        chainLen[verts[prev].chain]++;
        return verts.cnt() - 1;
    }

    void closeChain(int first, int last) {
        verts[first].prev = last;
        verts[last].next = first;
    }

    template<bool otherOnHeap>
    void copyFrom(Polygon<otherOnHeap>& other) {
        init();
        for(int i = 0; i < other.verts.cnt(); i++) {
            verts.add(other.verts[i]);
        }
        for(int i = 0; i < other.chains.cnt(); i++) {
            chains.add(other.chains[i]);
            chainLen.add(other.chainLen[i]);
        }
    }

    bool isEar(int i1) {
        int i0 = verts[i1].prev;
        int i2 = verts[i1].next;
        glm::vec2 p0 = verts[i0].pt;
        glm::vec2 p1 = verts[i1].pt;
        glm::vec2 p2 = verts[i2].pt;

        glm::vec2 p0p1 = p1 - p0;
        glm::vec2 p1p2 = p2 - p1;
        float cross = p0p1.x * p1p2.y - p1p2.x * p0p1.y; 
        if(cross <= 0.0f) {

            int curr = verts[i2].next;
            while(curr != i0) {

                if(ptInsideTri(verts[curr].pt, p0, p1, p2))
                    return false;

                curr = verts[curr].next;

            }

            return true;

        } 

        return false;    
    }

    void clip(int i1) {
        chainLen[verts[i1].chain]--;
        int i0 = verts[i1].prev;
        int i2 = verts[i1].next;
        verts[i0].next = i2;
        verts[i2].prev = i0;
        if(i1 == chains[verts[i1].chain]) {
            chains[verts[i1].chain] = i0;
        }
    }

    Arr<Vert, onHeap> verts;
    // first chain always exterior, rest are holes 
    // exterior goes cw, holes go ccw 
    Arr<int, onHeap> chains;
    Arr<int, onHeap> chainLen;

};

template<bool polyOnHeap, bool triArrOnHeap>
void triangulate(Polygon<polyOnHeap>& poly, Arr<int, triArrOnHeap>& tris) {

    Polygon<false> p;
    p.copyFrom(poly);

    while(p.chainLen[0] > 3) {

        bool foundEar = false;

        int curr = p.chains[0];
        do {
            if(p.isEar(curr)) {
                foundEar = true;
                tris.add(p.verts[curr].prev);
                tris.add(curr);
                tris.add(p.verts[curr].next);
                p.clip(curr);
                break;
            }
            curr = p.verts[curr].next;
        } while(curr != p.chains[0]);

        // only occurs for invalid polygons
        if(!foundEar)
            return;

    }

    int i1 = p.chains[0];
    int i0 = p.verts[i1].prev;
    int i2 = p.verts[i1].next;

    tris.add(i0);
    tris.add(i1);
    tris.add(i2);
        
}

#endif
