
#ifndef AUDIO_H
#define AUDIO_H

#include "../lib/miniaudio/miniaudio.h"

class Audio {

public:
    void init();    
    void play(const char* name);

private:
    ma_engine engine;

};

#endif
