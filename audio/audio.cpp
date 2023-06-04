
#define MINIAUDIO_IMPLEMENTATION
#include "../lib/miniaudio/miniaudio.h"
#include "audio.h"

void Audio::init() {
    ma_result res = ma_engine_init(NULL, &engine);
    if(res != MA_SUCCESS) {
        printf("miniaudio ded :(\n");
        exit(-1);
    }
}    

void Audio::play(const char* name) {
    char buf[512];
    sprintf(buf, "res/sfx/%s.wav", name);
    ma_engine_play_sound(&engine, buf, NULL);
}