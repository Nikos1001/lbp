
#include "player.h"
#include "main.h"
#include "input.h"

#define BOX_RADIUS 0.2f

static void initLimb(PlayerLimb* limb, float x, float y) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    limb->body = phys->CreateBody(&bodyDef);

    b2CircleShape shape;
    shape.m_radius = BOX_RADIUS;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.2f;

    limb->solid = limb->body->CreateFixture(&fixtureDef);

    fixtureDef.isSensor = true;
    fixtureDef.density = 0;
    shape.m_p = b2Vec2(0.0f, 0.05f);
    shape.m_radius -= 0.05f;
    limb->frontSensor = limb->body->CreateFixture(&fixtureDef);
    limb->backSensor = limb->body->CreateFixture(&fixtureDef);

    limb->body->SetFixedRotation(true);
}

void Player::init(float x, float y) {
    initLimb(&head, x, y + 0.5); 
    initLimb(&legs, x, y); 

    b2PrismaticJointDef prismatic;
    b2Vec2 worldAxis(0.0f, 1.0f);
    prismatic.Initialize(head.body, legs.body, legs.body->GetWorldCenter(), worldAxis);
    prismatic.lowerTranslation = 0.0f;
    prismatic.upperTranslation = 0.5f;
    prismatic.enableLimit = true;
    phys->CreateJoint(&prismatic);

    b2DistanceJointDef distance;
    distance.Initialize(head.body, legs.body, head.body->GetWorldCenter(), legs.body->GetWorldCenter());
    distance.frequencyHz = 15.0f;
    distance.dampingRatio = 1.2f;
    phys->CreateJoint(&distance);

    jumpBuffer.init(0.1, 0.1, 0.2);
    rising = false;
}

void Player::free() {
    phys->DestroyBody(head.body);
    phys->DestroyBody(legs.body);
}

void Player::render() {

    rnd.meshShader.use();

    glm::mat4 transform = glm::mat4(1.0f); 
    transform = glm::translate(transform, glm::vec3(head.body->GetPosition().x, head.body->GetPosition().y, -layer - 0.5f));
    transform = glm::scale(transform, glm::vec3(2 * BOX_RADIUS, 2 * BOX_RADIUS, 1.0f));
    rnd.meshShader.setMat4Uniform("uModel", transform);
    rnd.quad.render();

    transform = glm::mat4(1.0f); 
    transform = glm::translate(transform, glm::vec3(legs.body->GetPosition().x, legs.body->GetPosition().y, -layer - 0.5f));
    transform = glm::scale(transform, glm::vec3(2 * BOX_RADIUS, 2 * BOX_RADIUS, 1.0f));
    rnd.meshShader.setMat4Uniform("uModel", transform);
    rnd.quad.render();

}

#define PLAYER_SPEED 5
#define PLAYER_RUN_FORCE 1.1
#define PLAYER_MAX_RUN_FORCE 10

#define PLAYER_JUMP_FORCE 30
#define PLAYER_RISE_GRAVITY 0.7
#define PLAYER_HANG_GRAVITY 1.1
#define PLAYER_FALL_GRAVITY 1.6

void Player::update(float dt) {

    // Check for groundedness and front/back sensors
    bool grounded = false;
    bool objInFront = false;
    bool objBehind = false;
    for(b2ContactEdge* ce = legs.body->GetContactList(); ce; ce = ce->next) {
        b2Contact* c = ce->contact;
        if(c->IsTouching() && c->GetFixtureB() == legs.solid) {
            b2WorldManifold manifold;
            c->GetWorldManifold(&manifold);
            float dy = legs.body->GetWorldCenter().y - manifold.points[0].y;
            grounded |= dy < 0.2 && dy > 0.13;
        }
        if(c->IsTouching() && c->GetFixtureB() == legs.frontSensor)
            objInFront = true;
        if(c->IsTouching() && c->GetFixtureB() == legs.backSensor)
            objBehind = true;
    }
    for(b2ContactEdge* ce = head.body->GetContactList(); ce; ce = ce->next) {
        b2Contact* c = ce->contact;
        if(c->IsTouching() && c->GetFixtureB() == head.frontSensor)
            objInFront = true;
        if(c->IsTouching() && c->GetFixtureB() == head.backSensor)
            objBehind = true;
    }

    rnd.camPos.x = head.body->GetPosition().x;
    rnd.camPos.y = head.body->GetPosition().y;

    float horizInput = 0.0f;
    if(keyDown('A'))
        horizInput -= 1.0f;
    if(keyDown('D'))
        horizInput += 1.0f;
    
    float targetXSpeed = horizInput * PLAYER_SPEED;
    float currXSpeed = head.body->GetLinearVelocity().x;

    float runForce = PLAYER_RUN_FORCE * (targetXSpeed - currXSpeed);

    if(fabs(runForce) > PLAYER_MAX_RUN_FORCE)
        runForce = runForce / fabs(runForce) * PLAYER_MAX_RUN_FORCE;

    applyForce(glm::vec2(runForce, 0.0f));

    jumpBuffer.update(dt);
    jumpBuffer.setActionAvailable(grounded);
    if(keyPressed(' ')) 
        jumpBuffer.doAction();
    if(jumpBuffer.isActionTime()) {
        applyForce(glm::vec2(0.0f, PLAYER_JUMP_FORCE));
        audio.play("jump");
        rising = true;
    }

    float gravity;
    if(head.body->GetLinearVelocity().y > 0) {
        if(rising) {
            gravity = PLAYER_RISE_GRAVITY; 
            if(!keyDown(' '))
                rising = false;
        } else {
            gravity = PLAYER_HANG_GRAVITY;
        }
    } else {
        gravity = PLAYER_FALL_GRAVITY;
    }
    head.body->SetGravityScale(gravity);
    legs.body->SetGravityScale(gravity);

    if(keyPressed('W') && !objBehind && layer != MAX_LAYER)
        layer++;
    if(keyPressed('S') && !objInFront && layer != 0)
        layer--;

    uint32_t colMask = 1 << layer;
    b2Filter filter;
    filter.categoryBits = colMask;
    filter.maskBits = colMask;
    head.solid->SetFilterData(filter);
    legs.solid->SetFilterData(filter);

    uint32_t frontColMask = 1 << (layer - 1);
    if(layer == 0)
        frontColMask = 0;
    filter.categoryBits = frontColMask;
    filter.maskBits = frontColMask;
    head.frontSensor->SetFilterData(filter);
    legs.frontSensor->SetFilterData(filter);

    uint32_t backColMask = 1 << (layer + 1);
    filter.categoryBits = backColMask;
    filter.maskBits = backColMask;
    head.backSensor->SetFilterData(filter);
    legs.backSensor->SetFilterData(filter);

    if(head.body->GetPosition().y - legs.body->GetPosition().y < 0.35) {
        printf("Player crushed!\n");
        audio.play("squish");
        kill();
        return;
    }

}

void Player::applyForce(glm::vec2 force) {
    head.body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
    legs.body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
}

void Player::kill() {
    players.kill(this);
}
