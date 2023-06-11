
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

                glm::vec2 pt = verts[curr].pt;
                if(ptInsideTri(pt, p0, p1, p2) && pt != p0 && pt != p1 && pt != p2)
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

template<bool onHeap>
void simplifyChain(Polygon<onHeap>& poly, int chain, Arr<int, false>& newToOld) {

    int m = poly.chains[chain];
    int curr = m;
    do {
        if(poly.verts[curr].pt.x > poly.verts[m].pt.x)
            m = curr;
        curr = poly.verts[curr].next;
    } while(curr != poly.chains[chain]);
    glm::vec2 M = poly.verts[m].pt;

    glm::vec2 I = glm::vec2(999999999.0f);
    glm::vec2 P = glm::vec2(0);
    int p = -1;
    bool foundI = false;
    curr = poly.chains[0];
    do {
        int next = poly.verts[curr].next;
        glm::vec2 p0 = poly.verts[curr].pt;
        glm::vec2 p1 = poly.verts[next].pt; 

        float top = std::max(p0.y, p1.y);
        float bottom = std::min(p0.y, p1.y);
        if(top != bottom && M.y >= bottom && M.y <= top) {
            float t = (M.y - p0.y) / (p1.y - p0.y);
            float x = (1 - t) * p0.x + t * p1.x;
            if(x > M.x) {
                glm::vec2 newI = glm::vec2(x, M.y);
                if(glm::distance(M, newI) < glm::distance(M, I)) {
                    I = newI;
                    P = p0.x > p1.x ? p0 : p1;
                    p = p0.x > p1.x ? curr : next;
                    foundI = true;
                }
            }
        }

        curr = next;
    } while(curr != poly.chains[0]);

    // only happens in invalid polygons
    if(!foundI)
        return;

    int a = -1;
    float minDist = 99999999.9f;
    curr = poly.chains[0];
    do {
        glm::vec2 pt = poly.verts[curr].pt;
        if(ptInsideTri(pt, M, I, P)) { 
            float dist = glm::distance(M, pt);
            if(dist < minDist) {
                a = curr;
                minDist = dist;
            }
        }
        curr = poly.verts[curr].next;
    } while(curr != poly.chains[0]);
    p = a;

    int mCopy = poly.addPoint(poly.verts[m].pt + glm::vec2(-0.001f, 0.0f), poly.verts[m].prev);
    newToOld.add(newToOld[m]);
    poly.chainLen[0] += poly.chainLen[chain]; 
    poly.chainLen[chain] = 0;
    poly.verts[mCopy].chain = 0;
    int pCopy = poly.addPoint(poly.verts[p].pt + glm::vec2(-0.001f, 0.0f), mCopy);
    newToOld.add(newToOld[p]);
    int nextAfterP = poly.verts[p].next;
    poly.verts[p].next = m; 
    poly.verts[m].prev = p;
    poly.verts[pCopy].next = nextAfterP; 
    poly.verts[nextAfterP].prev = pCopy;

    curr = poly.chains[0];
    do {
        poly.verts[curr].chain = 0;
        curr = poly.verts[curr].next;
    } while(curr != poly.chains[0]);

}

struct FloatInt {
    float a;
    int b;
};

static int compFloatInt(const void* a_, const void* b_) {
    FloatInt* a = (FloatInt*)a_;
    FloatInt* b = (FloatInt*)b_;
    return a->a < b->a;
}

template<bool onHeap>
void simplify(Polygon<onHeap>& poly, Arr<int, false>& newToOld) {

    if(poly.chains.cnt() == 1)
        return;

    FloatInt chains[poly.chains.cnt()];
    for(int i = 1; i < poly.chains.cnt(); i++) {
        chains[i].a = -999999.0f;
        chains[i].b = i; 
        int curr = poly.chains[i];
        do {
            if(poly.verts[curr].pt.x > chains[i].a)
                chains[i].a = poly.verts[curr].pt.x;
            curr = poly.verts[curr].next;
        } while(curr != poly.chains[i]);
    }

    qsort(&chains[1], poly.chains.cnt() - 1, sizeof(FloatInt), compFloatInt);

    for(int i = 0; i < poly.chains.cnt() - 1; i++) {
        simplifyChain(poly, chains[1 + i].b, newToOld);
    }
}

template<bool polyOnHeap, bool triArrOnHeap>
void triangulate(Polygon<polyOnHeap>& poly, Arr<int, triArrOnHeap>& tris) {

    Polygon<false> p;
    p.copyFrom(poly);
    Arr<int, false> newToOld;
    newToOld.init();
    for(int i = 0; i < p.verts.cnt(); i++)
        newToOld.add(i);
    simplify(p, newToOld);

    while(p.chainLen[0] > 3) {

        bool foundEar = false;

        int curr = p.chains[0];
        do {
            if(p.isEar(curr)) {
                foundEar = true;
                tris.add(newToOld[p.verts[curr].prev]);
                tris.add(newToOld[curr]);
                tris.add(newToOld[p.verts[curr].next]);
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

    tris.add(newToOld[i0]);
    tris.add(newToOld[i1]);
    tris.add(newToOld[i2]);

}

template<bool onHeap>
void inset(Polygon<onHeap>& poly, float inset) {
    glm::vec2 bisectors[poly.verts.cnt()];
    for(int i = 0; i < poly.verts.cnt(); i++) {
        glm::vec2 p0 = poly.verts[poly.verts[i].prev].pt;
        glm::vec2 p1 = poly.verts[i].pt;
        glm::vec2 p2 = poly.verts[poly.verts[i].next].pt;

        glm::vec2 p1p0 = glm::normalize(p0 - p1);
        glm::vec2 p1p2 = glm::normalize(p2 - p1);

        glm::vec2 bisector = glm::normalize(p1p0 + p1p2);

        float angleBetween = glm::acos(glm::dot(p1p0, p1p2)); 
        bisector *= 1 / sin(angleBetween / 2);

        glm::vec3 prod = glm::cross(glm::vec3(p1p0, 0.0f), glm::vec3(p1p2, 0.0f));
        if(prod.z < 0)
            bisector *= -1; 
        bisectors[i] = bisector;
    }

    float vertInset[poly.verts.cnt()];
    for(int i = 0; i < poly.verts.cnt(); i++)
        vertInset[i] = inset; 
    for(int i = 0; i < poly.verts.cnt(); i++) {
        int i1 = i;
        int i2 = poly.verts[i1].next;
        glm::vec2 p1 = poly.verts[i1].pt;
        glm::vec2 p2 = poly.verts[i2].pt;
        glm::vec2 p1p2 = glm::normalize(p2 - p1);
        float edgeLen = glm::distance(p1, p2);
        float edgeShrinkSpeed = glm::dot(bisectors[i1], p1p2) + glm::dot(bisectors[i2], -p1p2); 
        float edgeDisappearTime = edgeLen / edgeShrinkSpeed; 
        edgeLen -= edgeShrinkSpeed * inset;
        if(edgeDisappearTime >= 0) {
            vertInset[i1] = std::min(vertInset[i1], edgeDisappearTime);
            vertInset[i2] = std::min(vertInset[i2], edgeDisappearTime);
        }
    }

    for(int i = 0; i < poly.verts.cnt(); i++) {
        poly.verts[i].pt += bisectors[i] * vertInset[i];
    }
}

#endif
