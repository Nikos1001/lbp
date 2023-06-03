
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
    glm::mat4 transform = rnd.projView; 
    transform = glm::translate(transform, glm::vec3(body->GetPosition().x, body->GetPosition().y, 0.0f));
    transform = glm::rotate(transform, body->GetAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
    rnd.meshShader.setMat4Uniform("uTrans", transform);
    mesh.render();
}

void Block::setDynamic(bool dynamic) {
    body->SetType(dynamic ? b2_dynamicBody : b2_staticBody);
}

static void generateBlockMesh(Polygon<true>& poly, Mesh& mesh) {
    Arr<int, false> tris;
    tris.init();
    triangulate(poly, tris);

    Arr<float, false> verts;
    verts.init();
    for(int i = 0; i < poly.verts.cnt(); i++) {
        verts.add(poly.verts[i].pt.x);
        verts.add(poly.verts[i].pt.y);
        verts.add(0.0f);
    }
    for(int i = 0; i < poly.verts.cnt(); i++) {
        verts.add(poly.verts[i].pt.x);
        verts.add(poly.verts[i].pt.y);
        verts.add(-1.0f);
    }
    for(int i = 0; i < poly.chains.cnt(); i++) {
        int i0 = poly.chains[i];
        do {
            int i1 = poly.verts[i0].next;
            int v0 = i0;
            int v1 = i1;
            int v2 = i0 + poly.verts.cnt();
            int v3 = i1 + poly.verts.cnt();

            tris.add(v0);
            tris.add(v1);
            tris.add(v2);
            tris.add(v3);
            tris.add(v1);
            tris.add(v2);

            i0 = i1; 
        } while(i0 != poly.chains[i]); 
    }

    mesh.upload(poly.verts.cnt() * 2, verts.elems, tris.cnt() / 3, (unsigned int*)tris.elems);
}

void Block::updateMesh() {

    generateBlockMesh(poly, mesh);

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
        fixtureDef.density = 0.5f;
        fixtureDef.friction = 0.3f;

        // TODO: destroy fixtures when editing mesh
        body->CreateFixture(&fixtureDef);
    }


}

