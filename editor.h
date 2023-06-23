
#ifndef EDITOR_H
#define EDITOR_H

#include "level.h"

class Editor {

public:

    void init();
    void update(float dt);

    Level level;

private:

    bool freecam;

    Arr<int, true> selectedPieces;
    void deselectAll();
    void selectPiece(int piece);

    bool polyEditMode;
    void polyEditModeUI();
    Arr<int, true> selectedVerts; 
    void selectVert(int vert);
    bool vertSelected(int vert);
    void deleteVert(LevelPiece* piece, int vert);
    glm::vec3 prevMousePos;

};

#endif
