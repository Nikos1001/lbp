
#ifndef MAIN_H
#define MAIN_H

#include "renderer/renderer.h"
#include "box2d.h"
#include "audio/audio.h"
#include "objList.h"
#include "block.h"
#include "player.h"

extern Renderer rnd; 
extern b2World* phys;
extern Audio audio;
extern ObjList<Block> blocks; 
extern ObjList<Player> players;

#define MAX_LAYER 2

#endif