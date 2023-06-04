
#include "main.h"
#include "block.h"
#include "player.h"
#include "renderer/texture.h"
#include "input.h"

Renderer rnd;
b2World* phys;
Audio audio;
ObjList<Block> blocks;
ObjList<Player> players;

int main() {

    arenaInit();

    rnd.init();

    b2Vec2 gravity(0.0f, -10.0f);
    phys = new b2World(gravity); 

    audio.init();

    blocks.init();
    players.init();

    Texture tex; 
    tex.init();
    tex.loadFromFile("res/mats/green-fabric/col.jpg");


    Block* block = blocks.spawn();
    block->frontLayer = 0;
    block->backLayer = 0;
    block->init(glm::vec2(2.5f, 2.0f));
    block->setDynamic(true);
    int start = block->poly.addChain(glm::vec2(-0.5f, 0.5f));
    int v = block->poly.addPoint(glm::vec2(0.0f, 0.0f), start);
    v = block->poly.addPoint(glm::vec2(0.5f, 0.5f), v);
    v = block->poly.addPoint(glm::vec2(0.5f, -0.5f), v);
    v = block->poly.addPoint(glm::vec2(0.0f, -0.15f), v);
    v = block->poly.addPoint(glm::vec2(-0.5f, -0.5f), v);
    block->poly.closeChain(start, v);
    block->updateMesh();

    Block* block2 = blocks.spawn();
    block2->frontLayer = 0;
    block2->backLayer = 1;
    block2->init(glm::vec2(-0.5f, -0.5f));
    start = block2->poly.addChain(glm::vec2(-0.25f, 0.25f));
    v = block2->poly.addPoint(glm::vec2(2.5f, 0.25f), start);
    v = block2->poly.addPoint(glm::vec2(25.0f, -0.25f), v);
    v = block2->poly.addPoint(glm::vec2(-0.25f, -0.25f), v);
    block2->poly.closeChain(start, v);
    block2->updateMesh();

    Block* block3 = blocks.spawn();
    block3->frontLayer = 1;
    block3->backLayer = 1;
    block3->init(glm::vec2(0.5f, 10.0f));
    block3->setDynamic(true);
    start = block3->poly.addChain(glm::vec2(-0.5f, 0.5f));
    v = block3->poly.addPoint(glm::vec2(0.0f, 0.75f), start);
    v = block3->poly.addPoint(glm::vec2(0.5f, 0.5f), v);
    v = block3->poly.addPoint(glm::vec2(0.5f, -0.5f), v);
    v = block3->poly.addPoint(glm::vec2(-0.5f, -0.5f), v);
    block3->poly.closeChain(start, v);
    block3->updateMesh();

    Player* player = players.spawn();
    player->layer = 1;
    player->init(0.5f, 0.0f);

    double prevTime = glfwGetTime(); 
    while(!glfwWindowShouldClose(rnd.window)) {
        double currTime = glfwGetTime();
        double dt = currTime - prevTime;
        prevTime = currTime;

        for(Player* curr = players.first(); curr; curr = players.next(curr)) {
            curr->update(dt);
        }

        char buf[128];
        sprintf(buf, "Frametime: %g", floor(dt * 1000));
        glfwSetWindowTitle(rnd.window, buf);

        phys->Step(1.0f / 60.0f, 9, 3);

        rnd.beginFrame();
        rnd.camPos = glm::vec3(0.0f, 1.0f, 3.0f);

        tex.use(0);
        rnd.meshShader.setIntUniform("uCol", 0);
        for(Block* curr = blocks.first(); curr; curr = blocks.next(curr)) {
            curr->render();
        }
        for(Player* curr = players.first(); curr; curr = players.next(curr)) {
            curr->render();
        }


        arenaClear();
        updateInput();
        rnd.endFrame();

    }

}
