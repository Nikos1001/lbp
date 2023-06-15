
#include "player.h"
#include "main.h"
#include "input.h"

#define CAPSULE_RADIUS 0.3f
#define CAPSULE_HEIGHT 0.3f

void CapsuleFixtures::init(b2Body* body, bool sensor) {
    b2FixtureDef fixtureDef;
    fixtureDef.density = sensor ? 0.0f : 1.0f;
    fixtureDef.isSensor = sensor;

    b2CircleShape headShape;
    headShape.m_radius = CAPSULE_RADIUS;
    if(sensor)
        headShape.m_radius -= 0.05f;
    headShape.m_p = b2Vec2(0.0f, CAPSULE_HEIGHT);
    fixtureDef.shape = &headShape;
    head = body->CreateFixture(&fixtureDef);

    b2CircleShape legShape;
    legShape.m_radius = CAPSULE_RADIUS;
    if(sensor)
        legShape.m_radius -= 0.05f;
    legShape.m_p = b2Vec2(0.0f, 0.0f);
    fixtureDef.shape = &legShape;
    legs = body->CreateFixture(&fixtureDef);

    b2PolygonShape bodyShape;
    float hx = CAPSULE_RADIUS;
    if(sensor)
        hx -= 0.05f;
    bodyShape.SetAsBox(hx, CAPSULE_HEIGHT / 2, b2Vec2(0.0f, CAPSULE_HEIGHT / 2), 0.0f);
    fixtureDef.shape = &bodyShape;
    torso = body->CreateFixture(&fixtureDef);
}

void CapsuleFixtures::setLayer(int layer) {
    uint32_t colMask = layer >= 0 ? 1 << layer : 0;
    b2Filter filter;
    filter.categoryBits = colMask;
    filter.maskBits = colMask; 
    head->SetFilterData(filter); 
    torso->SetFilterData(filter); 
    legs->SetFilterData(filter); 
}

bool CapsuleFixtures::containsFixture(b2Fixture* fixture) {
    return fixture == head || fixture == torso || fixture == legs;
}

void Player::init(float x, float y, int layer) {

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;
    bodyDef.position.Set(x, y);
    body = phys->CreateBody(&bodyDef);

    solid.init(body, false);
    crushSensor.init(body, true);
    frontSensor.init(body, true);
    backSensor.init(body, true);

    b2CircleShape grabSensorShape;
    grabSensorShape.m_radius = CAPSULE_RADIUS * 2;
    grabSensorShape.m_p = b2Vec2(0.0f, CAPSULE_HEIGHT);
    b2FixtureDef grabSensorDef;
    grabSensorDef.shape = &grabSensorShape;
    grabSensorDef.density = 0.0f;
    grabSensorDef.isSensor = true;
    grabSensor = body->CreateFixture(&grabSensorDef);

    prevAngle = 0;

    jumpBuffer.init(0.1, 0.1, 0.2);
    rising = false;

    this->layer = layer;

    grabbing = false;
    grabOther = NULL;
    grabJoint = NULL;
}

void Player::free() {
    phys->DestroyBody(body);
}

void Player::render() {

    rnd.meshShader.use();

    glm::mat4 transform = glm::mat4(1.0f); 
    transform = glm::translate(transform, glm::vec3(body->GetPosition().x, body->GetPosition().y, -layer - 0.5f));
    transform = glm::rotate(transform, body->GetAngle(), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, glm::vec3(0.0f, CAPSULE_HEIGHT / 2, 0.0f));
    transform = glm::scale(transform, glm::vec3(2 * CAPSULE_RADIUS, 2 * CAPSULE_RADIUS + CAPSULE_HEIGHT, 1.0f));
    rnd.meshShader.setMat4Uniform("uModel", transform);
    rnd.quad.render();

}

b2Vec2 getClosestPointOnFixture(b2Vec2 orig, b2Fixture* fixture) {
    b2Vec2 closest;
    float minDist = 999999999.0f;
    for(float angle = 0.0f; angle <= 2 * M_PI; angle += 0.1f) {
        b2RayCastInput in;
        in.p1 = orig;
        in.p2 = orig + b2Vec2(cosf(angle), sinf(angle));
        in.maxFraction = 100.0f;
        b2RayCastOutput out;
        fixture->GetShape()->RayCast(&out, in, fixture->GetBody()->GetTransform(), 0);
        if(out.fraction > 10.0f)
            continue;
        b2Vec2 point = in.p1 + out.fraction * (in.p2 - in.p1);
        float dist = b2Distance(orig, point);
        if(dist < minDist) {
            closest = point;
            minDist = dist;
        }
    }
    return closest;
}

GrabPoint Player::getGrabPoint() {
    GrabPoint res; 
    res.exists = false;
    res.dist = 9999999999.0f;
    b2Vec2 headPoint = body->GetWorldPoint(b2Vec2(0.0f, CAPSULE_HEIGHT));
    for(b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {
        b2Contact* c = ce->contact;
        if(c->IsTouching()) {
            if(c->GetFixtureB() == grabSensor) {
                b2Fixture* fixture = c->GetFixtureA();
                b2Vec2 pt = getClosestPointOnFixture(headPoint, fixture);
                float dist = b2Distance(pt, headPoint);
                if(dist < res.dist) {
                    res.dist = dist;
                    res.exists = true;
                    res.point = pt;
                    res.body = fixture->GetBody();
                }
            }
        }
    }
    return res;
}

#define PLAYER_STABLE_P 25.0f
#define PLAYER_STABLE_D 70.0f
#define PLAYER_STABLE_MAX_TORQUE 300.0f

#define PLAYER_SPEED 5
#define PLAYER_RUN_FORCE 15
#define PLAYER_MAX_RUN_FORCE 50
#define PLAYER_JUMP_RUN_FORCE 40

#define PLAYER_JUMP_FORCE 200
#define PLAYER_RISE_GRAVITY 0.7
#define PLAYER_HANG_GRAVITY 1.1
#define PLAYER_FALL_GRAVITY 1.6

void Player::update(float dt) {

    // Check for groundedness and front/back sensors
    bool grounded = false;
    bool objInFront = false;
    bool objBehind = false;
    bool crushed = false;
    for(b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {
        b2Contact* c = ce->contact;
        if(c->IsTouching()) {
            b2Fixture* fixture = c->GetFixtureB();
            b2Body* otherBody = c->GetFixtureA()->GetBody();
            if(fixture == solid.legs && otherBody != grabOther) {
                b2WorldManifold manifold;
                c->GetWorldManifold(&manifold);
                float dy = body->GetWorldCenter().y - manifold.points[0].y;
                grounded |= dy < 1.5 * CAPSULE_RADIUS && dy > 1.25 * CAPSULE_RADIUS;
            }
            if(crushSensor.containsFixture(fixture)) {
                crushed = true;
            }
            if(frontSensor.containsFixture(fixture)) {
                objInFront = true;
            }
            if(backSensor.containsFixture(fixture))
                objBehind = true;
        }
    }

    if(crushed) {
        audio.play("squish");
        kill();
        return;
    }

    // Camera tracking
    rnd.camPos.x = body->GetPosition().x;
    rnd.camPos.y = body->GetPosition().y + 1;

    // Rotation stabilization
    float angle = body->GetAngle();
    float dAngle = angle - prevAngle;
    float stableTorque = -angle * PLAYER_STABLE_P - dAngle * PLAYER_STABLE_D;
    if(fabs(stableTorque) > PLAYER_STABLE_MAX_TORQUE)
        stableTorque = PLAYER_STABLE_MAX_TORQUE * stableTorque / fabs(stableTorque);
    body->ApplyTorque(stableTorque, true);
    prevAngle = angle;

    // Running
    float horizInput = 0.0f;
    if(keyDown('A'))
        horizInput -= 1.0f;
    if(keyDown('D'))
        horizInput += 1.0f;
    
    float targetXSpeed = horizInput * PLAYER_SPEED;
    float currXSpeed = body->GetLinearVelocity().x;

    float runForce = PLAYER_RUN_FORCE * (targetXSpeed - currXSpeed);

    float maxForce = grounded ? PLAYER_MAX_RUN_FORCE : PLAYER_JUMP_RUN_FORCE;
    if(fabs(runForce) > maxForce)
        runForce = runForce / fabs(runForce) * maxForce;

    applyForce(glm::vec2(runForce, 0.0f));

    // Jumping

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
    if(body->GetLinearVelocity().y > 0) {
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
    body->SetGravityScale(gravity);
    body->SetGravityScale(gravity);

    // Layer switching
    if(keyPressed('W') && !objBehind && layer != MAX_LAYER && !grabbing)
        layer++;
    if(keyPressed('S') && !objInFront && layer != 0 && !grabbing)
        layer--;


    solid.setLayer(layer); 
    crushSensor.setLayer(layer);
    frontSensor.setLayer(layer - 1); 
    backSensor.setLayer(layer + 1); 

    uint32_t grabSensorFilterMask = 1 << layer;
    b2Filter grabSensorFilter;
    grabSensorFilter.categoryBits = grabSensorFilterMask;
    grabSensorFilter.maskBits = grabSensorFilterMask;
    grabSensor->SetFilterData(grabSensorFilter);

    // Grabbing
    bool shouldGrab = keyDown('O');
    if(grabbing) {
        if(!shouldGrab) {
            grabbing = false;
            grabOther = NULL;
            phys->DestroyJoint(grabJoint);
            grabJoint = NULL;
        }
    } else {
        if(keyPressed('O')) {
            GrabPoint point = getGrabPoint();
            if(point.exists) {
                grabbing = true;
                grabOther = point.body;

                b2DistanceJointDef distDef;
                b2Vec2 headPoint = body->GetWorldPoint(b2Vec2(0.0f, CAPSULE_HEIGHT));
                distDef.Initialize(body, point.body, headPoint, point.point);
                distDef.collideConnected = true;
                distDef.dampingRatio = 1.5f;
                distDef.frequencyHz = 5.0f;
                grabJoint = (b2DistanceJoint*)phys->CreateJoint(&distDef);
            }
        }
    }

}

void Player::applyForce(glm::vec2 force) {
    body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
}

void Player::kill() {
    ObjList<Player>::kill(this);
}
