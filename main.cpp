
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

    Player* player = players.spawn();
    player->init(-2.0f, 1.0f, 1);

    loadLevel("test");

    double prevTime = glfwGetTime(); 
    while(!glfwWindowShouldClose(rnd.window)) {

        arenaClear();
        double currTime = glfwGetTime();
        double dt = currTime - prevTime;
        prevTime = currTime;

        phys->Step(1.0f / 60.0f, 9, 3);

        for(Player* curr = players.first(); curr; curr = players.next(curr)) {
            curr->update(dt);
        }

        char buf[128];
        sprintf(buf, "Frametime: %g", floor(dt * 1000));
        glfwSetWindowTitle(rnd.window, buf);


        rnd.camPos.z = 3.0f; 
        rnd.beginFrame();

        rnd.beginLighting();

        rnd.setAmbient(glm::vec3(1.0f, 1.0f, 1.5f));
        rnd.setDirectional(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.6f, 0.6f, 0.8f));
        for(Player* curr = players.first(); curr; curr = players.next(curr)) { 
            rnd.addPointLight(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec3(10.0f));
        }

        rnd.endLighting();

        for(Block* curr = blocks.first(); curr; curr = blocks.next(curr)) {
            curr->render();
        }

        for(Player* curr = players.first(); curr; curr = players.next(curr)) {
            curr->render();
        }

        updateInput();
        rnd.endFrame();

    }

}
