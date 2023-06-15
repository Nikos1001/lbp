
#include "block.h"
#include "main.h"
#include "material.h"

void Block::init(bool dynamic) {
    b2BodyDef bodyDef;
    bodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.bullet = true;
    bodyDef.position.Set(0.0f, 0.0f);
    body = phys->CreateBody(&bodyDef);
    pieces.init();
}

void Block::free() {
    phys->DestroyBody(body);
    for(int i = 0; i < pieces.cnt(); i++) {
        if(!pieces[i].isModelMat) {
            pieces[i].mesh.free();
            pieces[i].poly.free();
        }
    }
    pieces.free();
}

void Block::render() {
    rnd.meshShader.use();
    glm::mat4 transform = glm::mat4(1.0f); 
    transform = glm::translate(transform, glm::vec3(body->GetPosition().x, body->GetPosition().y, 0.0f));
    transform = glm::rotate(transform, body->GetAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
    for(int i = 0; i < pieces.cnt(); i++) {
        int material = pieces[i].material;
        if(!pieces[i].isModelMat) {
            rnd.renderMesh(pieces[i].mesh, materials[material].col, materials[material].norm, transform);
        } else {
            glm::mat4 trans = glm::mat4(1.0f);
            glm::vec2 localPos = pieces[i].pos;
            b2Vec2 worldPos = body->GetWorldPoint(b2Vec2(localPos.x, localPos.y));
            trans = glm::translate(trans, glm::vec3(worldPos.x, worldPos.y, -pieces[i].frontLayer - 1.0f));
            trans = glm::rotate(trans, body->GetAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
            rnd.renderMesh(modelMaterials[material].mesh, modelMaterials[material].col, modelMaterials[material].norm, trans);
        }
    }
}

static void generateBlockMesh(Polygon<true>& poly, Mesh& mesh, int frontLayer, int backLayer, int material) {
    Polygon<false> insetPoly;
    insetPoly.copyFrom(poly);
    inset(insetPoly, materials[material].bevelWidth);
    Arr<int, false> tris;
    tris.init();
    triangulate(insetPoly, tris);

    Arr<MeshVert, false> verts;
    verts.init();

    // Generate face
    float faceZ = -frontLayer;
    if(materials[material].meshGen == SQUARE_BEVEL)
        faceZ -= materials[material].faceInset;
    for(int i = 0; i < poly.verts.cnt(); i++) {
        verts.add((MeshVert){glm::vec3(insetPoly.verts[i].pt, faceZ), insetPoly.verts[i].pt, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)});
    }
    
    // Generate borders
    for(int i = 0; i < poly.chains.cnt(); i++) {
        int i0 = poly.chains[i];
        float perim = 0.0f;
        do {
            int i1 = poly.verts[i0].next;

            glm::vec2 p0 = poly.verts[i0].pt;
            glm::vec2 p1 = poly.verts[i1].pt;
            glm::vec2 innerP0 = insetPoly.verts[i0].pt;
            glm::vec2 innerP1 = insetPoly.verts[i1].pt;

            float newPerim = perim + glm::distance(p0, p1);

            glm::vec3 tang = glm::normalize(glm::vec3(p1 - p0, 0.0f)); 
            glm::vec3 norm = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), tang));

            float frontZ = -(float)frontLayer;
            if(materials[material].meshGen == FLAT) {
                frontZ -= materials[material].bevelWidth;
            }
            float backZ = -(float)backLayer - 1.0f;

            // Border
            int v0 = verts.add((MeshVert){glm::vec3(p0, frontZ), glm::vec2(perim, 0.0f), norm, tang});
            int v1 = verts.add((MeshVert){glm::vec3(p1, frontZ), glm::vec2(newPerim, 0.0f), norm, tang});
            int v2 = verts.add((MeshVert){glm::vec3(p0, backZ), glm::vec2(perim, backZ - frontZ), norm, tang});
            int v3 = verts.add((MeshVert){glm::vec3(p1, backZ), glm::vec2(newPerim, backZ - frontZ), norm, tang});

            tris.add(v0);
            tris.add(v1);
            tris.add(v2);
            tris.add(v3);
            tris.add(v1);
            tris.add(v2);

            // Bevel
            if(materials[material].meshGen == FLAT) {
                glm::vec3 bitang = glm::normalize(glm::vec3(p0, frontZ) - glm::vec3(innerP0, faceZ));
                glm::vec3 bevelNorm = glm::normalize(glm::cross(tang, bitang));
                int v4 = verts.add((MeshVert){glm::vec3(innerP0, faceZ), glm::vec2(perim, 0.0f), bevelNorm, tang});
                int v5 = verts.add((MeshVert){glm::vec3(innerP1, faceZ), glm::vec2(newPerim, 0.0f), bevelNorm, tang});
                int v6 = verts.add((MeshVert){glm::vec3(p0, frontZ), glm::vec2(perim, 1.0f), bevelNorm, tang});
                int v7 = verts.add((MeshVert){glm::vec3(p1, frontZ), glm::vec2(newPerim, 1.0f), bevelNorm, tang});
                tris.add(v4);
                tris.add(v5);
                tris.add(v6);
                tris.add(v7);
                tris.add(v5);
                tris.add(v6);
            }
            if(materials[material].meshGen == SQUARE_BEVEL) {
                glm::vec3 outP0 = glm::vec3(p0, frontZ);
                glm::vec3 outP1 = glm::vec3(p1, frontZ);
                glm::vec3 inFP0 = glm::vec3(innerP0, frontZ);
                glm::vec3 inFP1 = glm::vec3(innerP1, frontZ);
                glm::vec3 inBP0 = glm::vec3(innerP0, faceZ);
                glm::vec3 inBP1 = glm::vec3(innerP1, faceZ);

                int v4 = verts.add((MeshVert){outP0, p0, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)});
                int v5 = verts.add((MeshVert){outP1, p1, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)});
                int v6 = verts.add((MeshVert){inFP0, innerP0, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)});
                int v7 = verts.add((MeshVert){inFP1, innerP1, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)});
                tris.add(v4);
                tris.add(v5);
                tris.add(v6);
                tris.add(v7);
                tris.add(v5);
                tris.add(v6); 

                glm::vec3 bitang = glm::vec3(0.0f, 0.0f, -1.0f);
                glm::vec3 norm = glm::cross(bitang, tang);
                int v8 = verts.add((MeshVert){inFP0, glm::vec2(perim, 1.0f), norm, tang});
                int v9 = verts.add((MeshVert){inFP1, glm::vec2(newPerim, 1.0f), norm, tang});
                int v10 = verts.add((MeshVert){inBP0, glm::vec2(perim, 0.0f), norm, tang});
                int v11 = verts.add((MeshVert){inBP1, glm::vec2(newPerim, 0.0f), norm, tang});
                tris.add(v8);
                tris.add(v9);
                tris.add(v10);
                tris.add(v11);
                tris.add(v9);
                tris.add(v10); 
            }

            i0 = i1; 
            perim = newPerim;
        } while(i0 != poly.chains[i]); 
    }

    mesh.upload(verts.cnt(), verts.elems, tris.cnt() / 3, (unsigned int*)tris.elems);
}

void updatePiece(BlockPiece& piece, b2Body* body) {

    if(!piece.isModelMat) {
        generateBlockMesh(piece.poly, piece.mesh, piece.frontLayer, piece.backLayer, piece.material);
    }

    Arr<int, false> tris;
    tris.init();
    if(!piece.isModelMat) {
        triangulate(piece.poly, tris);
    } else {
        triangulate(modelMaterials[piece.material].polygon, tris);
    }

    for(int i = 0; i < tris.cnt(); i += 3) {
        int i0 = tris[i];
        int i1 = tris[i + 1];
        int i2 = tris[i + 2];

        glm::vec2 p0, p1, p2;
        if(!piece.isModelMat) {
            p0 = piece.poly.verts[i0].pt;
            p1 = piece.poly.verts[i1].pt;
            p2 = piece.poly.verts[i2].pt;
        } else {
            int mat = piece.material;
            p0 = modelMaterials[mat].polygon.verts[i0].pt;
            p1 = modelMaterials[mat].polygon.verts[i1].pt;
            p2 = modelMaterials[mat].polygon.verts[i2].pt;

            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(piece.pos, 0.0f));

            p0 = transform * glm::vec4(p0, 0.0f, 1.0f);
            p1 = transform * glm::vec4(p1, 0.0f, 1.0f);
            p2 = transform * glm::vec4(p2, 0.0f, 1.0f);
        }

        float area = triArea(p0, p1, p2);
        if(area < 0.0001f)
            continue;

        b2PolygonShape shape;
        b2Vec2 points[3];
        points[0].Set(p0.x, p0.y);
        points[1].Set(p1.x, p1.y);
        points[2].Set(p2.x, p2.y);
        shape.Set(points, 3);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = materials[piece.material].density; 
        fixtureDef.friction = 0.3f;

        uint32_t mask = 0;
        for(int i = piece.frontLayer; i <= piece.backLayer; i++)
            mask |= 1 << i;
            
        fixtureDef.filter.categoryBits = mask;
        fixtureDef.filter.maskBits = mask;

        if(area > 0.0001f)
            body->CreateFixture(&fixtureDef);
    }


}

void Block::updateMesh() {
    for(b2Fixture* fixture = body->GetFixtureList(); fixture;) {
        b2Fixture* next = fixture->GetNext();
        body->DestroyFixture(fixture);
        fixture = next;
    }
    for(int i = 0; i < pieces.cnt(); i++) {
        updatePiece(pieces[i], body);
    }
}

int Block::addPiece(bool modelMat, int frontLayer, int backLayer, int material) {
    BlockPiece piece;
    piece.isModelMat = modelMat;
    piece.frontLayer = frontLayer;
    piece.backLayer = backLayer;
    piece.material = material;
    if(!modelMat) {
        piece.mesh.init();
        piece.poly.init();
    }
    pieces.add(piece);
    return pieces.cnt() - 1;
}
