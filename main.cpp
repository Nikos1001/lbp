
#include "main.h"
#include "block.h"
#include "player.h"
#include "renderer/texture.h"
#include "input.h"
#include "level.h"
#include "material.h"
#include "common/resManager.h"
#include "assets.h"
#include "imgui.h"
#include "editor.h"

b2World* phys;
Audio audio;
ObjList<Block> blocks;
ObjList<Player> players;
ObjList<Bolt> bolts;

int main() {

    arenaInit();

    Renderer::init();

    b2Vec2 gravity(0.0f, -10.0f);
    phys = new b2World(gravity); 

    audio.init();

    ObjList<Block>::init();
    ObjList<Player>::init();
    ObjList<Bolt>::init();

    loadMaterials();

    Player* player = players.spawn();
    player->init(-2.0f, 1.0f, 1);

    loadAssets();

    Editor editor;
    editor.init();

    double prevTime = glfwGetTime(); 
    while(!glfwWindowShouldClose(Renderer::window)) {

        arenaClear();
        double currTime = glfwGetTime();
        double dt = currTime - prevTime;
        prevTime = currTime;

        Renderer::camPos.z = 3.0f; 
        Renderer::beginFrame();

        phys->Step(1.0f / 60.0f, 9, 3);

        for(Player* curr = ObjList<Player>::first(); curr; curr = ObjList<Player>::next(curr)) {
            curr->update(dt);
        }

        Renderer::beginLighting();
        Renderer::endLighting();

        Renderer::beginScene();
        for(Block* curr = ObjList<Block>::first(); curr; curr = ObjList<Block>::next(curr)) {
            curr->render();
        }
        for(Player* curr = ObjList<Player>::first(); curr; curr = ObjList<Player>::next(curr)) {
            curr->render();
        }
        for(Bolt* curr = ObjList<Bolt>::first(); curr; curr = ObjList<Bolt>::next(curr)) {
            curr->render();
        }
        Renderer::endScene();

        Renderer::beginUI();
        editor.update(dt);
        Renderer::endUI();

        updateInput();
        Renderer::endFrame();

    }

}
