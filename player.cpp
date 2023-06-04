
#include "player.h"
#include "main.h"
#include "input.h"

#define BOX_RADIUS 0.2f

static b2Body* makeBox(float x, float y) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    b2Body* body = phys->CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = BOX_RADIUS;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 1.0f;

    body->CreateFixture(&fixtureDef);
    body->SetFixedRotation(true);

    return body;
}

void Player::init(float x, float y) {
    head = makeBox(x, y + 0.5); 
    legs = makeBox(x, y); 

    b2PrismaticJointDef prismatic;
    b2Vec2 worldAxis(0.0f, 1.0f);
    prismatic.Initialize(head, legs, legs->GetWorldCenter(), worldAxis);
    prismatic.lowerTranslation = 0.0f;
    prismatic.upperTranslation = 0.5f;
    prismatic.enableLimit = true;
    phys->CreateJoint(&prismatic);

    b2DistanceJointDef distance;
    distance.Initialize(head, legs, head->GetWorldCenter(), legs->GetWorldCenter());
    distance.frequencyHz = 15.0f;
    distance.dampingRatio = 0.85f;
    phys->CreateJoint(&distance);
}

void Player::free() {
    phys->DestroyBody(head);
    phys->DestroyBody(legs);
}

void Player::render() {

    rnd.meshShader.use();
    glm::mat4 transform = glm::mat4(1.0f); 
    transform = glm::translate(transform, glm::vec3(head->GetPosition().x, head->GetPosition().y, -layer - 0.5f));
    transform = glm::scale(transform, glm::vec3(2 * BOX_RADIUS, 2 * BOX_RADIUS, 1.0f));
    rnd.meshShader.setMat4Uniform("uModel", transform);
    rnd.quad.render();

    transform = glm::mat4(1.0f); 
    transform = glm::translate(transform, glm::vec3(legs->GetPosition().x, legs->GetPosition().y, -layer - 0.5f));
    transform = glm::scale(transform, glm::vec3(2 * BOX_RADIUS, 2 * BOX_RADIUS, 1.0f));
    rnd.meshShader.setMat4Uniform("uModel", transform);
    rnd.quad.render();

}

void Player::update(float dt) {

    rnd.camPos.x = head->GetPosition().x;
    rnd.camPos.y = head->GetPosition().y;

    float horizInput = 0.0f;
    if(keyDown('A'))
        horizInput -= 1.0f;
    if(keyDown('D'))
        horizInput += 1.0f;
    
    head->ApplyForceToCenter(b2Vec2(horizInput, 0.0f), true);
    legs->ApplyForceToCenter(b2Vec2(horizInput, 0.0f), true);

    if(head->GetPosition().y - legs->GetPosition().y < 0.35) {
        printf("Player crushed!\n");
    }

    bool grounded = false;
    for(b2ContactEdge* ce = legs->GetContactList(); ce; ce = ce->next) {
        b2Contact* c = ce->contact;
        if(c->IsTouching()) {
            grounded |= c->GetManifold()->localNormal.y > 0.95;
        }
    }

    if(grounded) {
        if(keyDown(' ')) {
            head->ApplyForceToCenter(b2Vec2(0.0f, 10.0f), true);
            legs->ApplyForceToCenter(b2Vec2(0.0f, 10.0f), true);
        }
    }

    if(keyPressed('W'))
        layer++;
    if(keyPressed('S'))
        layer--;

    uint32_t colMask = 1 << layer;
    b2Filter filter;
    filter.categoryBits = colMask;
    filter.maskBits = colMask;
    for(b2Fixture* f = head->GetFixtureList(); f; f = f->GetNext()) {
        f->SetFilterData(filter);
    }
    for(b2Fixture* f = legs->GetFixtureList(); f; f = f->GetNext()) {
        f->SetFilterData(filter);
    }

}
