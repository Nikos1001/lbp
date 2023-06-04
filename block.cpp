
#include "block.h"
#include "main.h"

void Block::init(glm::vec2 pos) {
    mesh.init();
    poly.init();

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set(pos.x, pos.y);
    body = phys->CreateBody(&bodyDef);
}

void Block::free() {
    mesh.free();
    poly.free();
    phys->DestroyBody(body);
}

void Block::render() {
    rnd.meshShader.use();
    glm::mat4 transform = glm::mat4(1.0f); 
    transform = glm::translate(transform, glm::vec3(body->GetPosition().x, body->GetPosition().y, 0.0f));
    transform = glm::rotate(transform, body->GetAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
    rnd.meshShader.setMat4Uniform("uModel", transform);
    mesh.render();
}

void Block::setDynamic(bool dynamic) {
    body->SetType(dynamic ? b2_dynamicBody : b2_staticBody);
}

static void generateBlockMesh(Polygon<true>& poly, Mesh& mesh, int frontLayer, int backLayer) {
    Arr<int, false> tris;
    tris.init();
    triangulate(poly, tris);

    Arr<MeshVert, false> verts;
    verts.init();
    for(int i = 0; i < poly.verts.cnt(); i++) {
        verts.add((MeshVert){glm::vec3(poly.verts[i].pt, -frontLayer), poly.verts[i].pt, glm::vec3(0.0f, 0.0f, 1.0f)});
    }
    for(int i = 0; i < poly.chains.cnt(); i++) {
        int i0 = poly.chains[i];
        float perim = 0.0f;
        do {
            int i1 = poly.verts[i0].next;

            glm::vec2 p0 = poly.verts[i0].pt;
            glm::vec2 p1 = poly.verts[i1].pt;

            float newPerim = perim + glm::distance(p0, p1);

            glm::vec3 norm = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(p1 - p0, 0.0f)));

            int v0 = verts.add((MeshVert){glm::vec3(p0, -(float)frontLayer), glm::vec2(perim, 0.0f), norm});
            int v1 = verts.add((MeshVert){glm::vec3(p1, -(float)frontLayer), glm::vec2(newPerim, 0.0f), norm});
            int v2 = verts.add((MeshVert){glm::vec3(p0, -(float)backLayer - 1.0f), glm::vec2(perim, backLayer - frontLayer + 1), norm});
            int v3 = verts.add((MeshVert){glm::vec3(p1, -(float)backLayer - 1.0f), glm::vec2(newPerim, backLayer - frontLayer + 1), norm});

            tris.add(v0);
            tris.add(v1);
            tris.add(v2);
            tris.add(v3);
            tris.add(v1);
            tris.add(v2);

            i0 = i1; 
            perim = newPerim;
        } while(i0 != poly.chains[i]); 
    }

    mesh.upload(verts.cnt(), verts.elems, tris.cnt() / 3, (unsigned int*)tris.elems);
}

void Block::updateMesh() {

    generateBlockMesh(poly, mesh, frontLayer, backLayer);

    Arr<int, false> tris;
    tris.init();
    triangulate(poly, tris);

    for(int i = 0; i < tris.cnt(); i += 3) {
        int i0 = tris[i];
        int i1 = tris[i + 1];
        int i2 = tris[i + 2];

        glm::vec2 p0 = poly.verts[i0].pt;
        glm::vec2 p1 = poly.verts[i1].pt;
        glm::vec2 p2 = poly.verts[i2].pt;

        b2PolygonShape shape;
        b2Vec2 points[3];
        points[0].Set(p0.x, p0.y);
        points[1].Set(p1.x, p1.y);
        points[2].Set(p2.x, p2.y);
        shape.Set(points, 3);

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 500.0f;
        fixtureDef.friction = 0.3f;

        uint32_t mask = 0;
        for(int i = frontLayer; i <= backLayer; i++)
            mask |= 1 << i;
            
        fixtureDef.filter.categoryBits = mask;
        fixtureDef.filter.maskBits = mask;

        // TODO: destroy fixtures when editing mesh
        body->CreateFixture(&fixtureDef);
    }


}

