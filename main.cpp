
#include "main.h"
#include "block.h"
#include "player.h"
#include "renderer/texture.h"
#include "input.h"
#include "level.h"
#include "material.h"

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

    loadMaterials();
    loadLevel("test");

    Texture tex; 
    tex.init();
    tex.loadFromFile("res/mats/green-fabric/col.jpg");

    Player* player = players.spawn();
    player->layer = 1;
    player->init(-10.0f, 1.0f);

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

        rnd.camPos.z = 3.0f; 
        rnd.beginFrame();

        for(Block* curr = blocks.first(); curr; curr = blocks.next(curr)) {
            curr->render();
        }

        tex.use(0);
        rnd.meshShader.setIntUniform("uCol", 0);
        for(Player* curr = players.first(); curr; curr = players.next(curr)) {
            curr->render();
        }


        arenaClear();
        updateInput();
        rnd.endFrame();

    }

}
