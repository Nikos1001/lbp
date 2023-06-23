
#include "editor.h"
#include "imgui.h"
#include "renderer/renderer.h"
#include "input.h"
#include "material.h"
#include "player.h"
#include "objList.h"
#include "main.h"

const glm::vec3 uiGreen = glm::vec3(0.3f, 1.0f, 0.4f);

void Editor::init() {
    level.init();

    freecam = true;

    selectedPieces.init();

    polyEditMode = false;
    selectedVerts.init();
}

static bool editorKeyPressed(ImGuiKey key) {
    if(ImGui::GetIO().WantCaptureKeyboard)
        return false;
    return ImGui::IsKeyPressed(key);
}

static bool editorMousePressed() {
    if(ImGui::GetIO().WantCaptureMouse)
        return false;
    return ImGui::IsMouseClicked(ImGuiMouseButton_Left);
}

static bool editorMouseReleased() {
    if(ImGui::GetIO().WantCaptureMouse)
        return false;
    return ImGui::IsMouseReleased(ImGuiMouseButton_Left);
}

static bool editorMouseDragging() {
    if(ImGui::GetIO().WantCaptureMouse)
        return false;
    return ImGui::IsMouseDragging(ImGuiMouseButton_Left);
}

static bool editorKeyDown(ImGuiKey key) {
    if(ImGui::GetIO().WantCaptureKeyboard)
        return false;
    return ImGui::IsKeyDown(key);
}

void Editor::update(float dt) {
    ImGui::Begin("Level Settings");
    ImGui::Text("Polygons: %d / %d", level.activePieces, level.pieces.cnt());
    ImGui::Text("Groups: %d / %d", level.activeBlocks, level.blocks.cnt());
    ImGui::ColorEdit3("Sky Color", (float*)&level.skyCol, ImGuiColorEditFlags_NoInputs);
    ImGui::ColorEdit3("Sun Color", (float*)&level.dirCol, ImGuiColorEditFlags_NoInputs);
    ImGui::End();

    ImGui::Begin("Tools");
    ImGui::BeginDisabled(polyEditMode);
    if(ImGui::Button("Add Polygon")) {
        int block = level.addBlock();
        int piece = level.addPiece(block);
        glm::vec3 camPos = Renderer::camPos;
        level.pieces[piece].vertices.add(makePointVertex(glm::vec2(camPos.x - 0.5f, camPos.y - 2.0f)));
        level.pieces[piece].vertices.add(makePointVertex(glm::vec2(camPos.x + 0.5f, camPos.y - 2.0f)));
        level.pieces[piece].vertices.add(makePointVertex(glm::vec2(camPos.x + 0.5f, camPos.y - 3.0f)));
        level.pieces[piece].vertices.add(makePointVertex(glm::vec2(camPos.x - 0.5f, camPos.y - 3.0f)));
        level.blocks[block].update(&level);
        deselectAll();
        selectPiece(piece);
    }
    ImGui::EndDisabled();

    if(!polyEditMode && selectedPieces.cnt() > 0 && (editorKeyPressed(ImGuiKey_Delete) || editorKeyPressed(ImGuiKey_Backspace))) {
        for(int i = 0; i < selectedPieces.cnt(); i++) {
            level.deletePiece(selectedPieces[i]);
        }
        deselectAll();
    }

    ImGui::Checkbox("Freecam", &freecam);
    ImGui::End();

    if(selectedPieces.cnt() > 0) {
        ImGui::Begin("Polygon Settings");

        int material = level.pieces[selectedPieces[0]].material;
        bool materialsMixed = false;
        for(int i = 1; i < selectedPieces.cnt(); i++) {
            if(material != level.pieces[selectedPieces[0]].material) {
                materialsMixed = true;
            }
        } 
        ImGui::Text("Material: ");
        ImGui::SameLine();
        if(ImGui::Button(materialsMixed ? "Mixed" : materials[material].name)) {
            ImGui::OpenPopup("material_selection_popup");
        } 
        if(ImGui::BeginPopup("material_selection_popup")) {
            int chosenMaterial = -1;
            for(int i = 0; i < materials.cnt(); i++) {
                if(ImGui::Selectable(materials[i].name)) {
                    chosenMaterial = i;
                }
            }
            if(chosenMaterial != -1) {
                for(int i = 0; i < selectedPieces.cnt(); i++) {
                    level.pieces[selectedPieces[i]].material = chosenMaterial;
                    level.blocks[level.pieces[selectedPieces[i]].block].update(&level);
                }
            }
            ImGui::EndPopup();
        }

        bool editPolyButton = ImGui::Button(polyEditMode ? "Exit Polygon Editing" : "Edit Polygon Shape");
        if(editPolyButton || editorKeyPressed(ImGuiKey_Tab)) {
            if(polyEditMode) {
                selectedVerts.clear();
                polyEditMode = false;
            } else {
                int firstPiece = selectedPieces[0];
                selectedPieces.clear();
                selectedPieces.add(firstPiece);
                polyEditMode = true;
            }
        }

        int layer = level.pieces[selectedPieces[0]].frontLayer;
        if(ImGui::SliderInt("##", &layer, 0, MAX_LAYER, "Layer %d")) {
            for(int i = 0; i < selectedPieces.cnt(); i++) {
                int thickness = level.pieces[selectedPieces[i]].backLayer - level.pieces[selectedPieces[i]].frontLayer;
                level.pieces[selectedPieces[i]].frontLayer = layer;
                level.pieces[selectedPieces[i]].backLayer = std::min(layer + thickness, MAX_LAYER);
                level.blocks[level.pieces[selectedPieces[i]].block].update(&level);
            }
        }
        int thickness = level.pieces[selectedPieces[0]].backLayer - level.pieces[selectedPieces[0]].frontLayer;
        if(ImGui::SliderInt("##x", &thickness, 0, MAX_LAYER, "Thickness %d")) {
            for(int i = 0; i < selectedPieces.cnt(); i++) {
                level.pieces[selectedPieces[i]].backLayer = std::min(level.pieces[selectedPieces[i]].frontLayer + thickness, MAX_LAYER);
                level.blocks[level.pieces[selectedPieces[i]].block].update(&level);
            }
        }

        ImGui::End();
    }

    if(freecam) {
        b2Vec2 camMovement(0.0f, 0.0f); 
        if(editorKeyDown(ImGuiKey_D))
            camMovement.x += 1.0f; 
        if(editorKeyDown(ImGuiKey_A))
            camMovement.x -= 1.0f;
        if(editorKeyDown(ImGuiKey_W))
            camMovement.y += 1.0f; 
        if(editorKeyDown(ImGuiKey_S))
            camMovement.y -= 1.0f;
        camMovement *= 5.0f * dt;

        for(Player* curr = ObjList<Player>::first(); curr; curr = ObjList<Player>::next(curr)) {
            curr->enable = false;
            curr->body->SetTransform(curr->body->GetPosition() + camMovement, 0.0f);
        }
    } else {
        for(Player* curr = ObjList<Player>::first(); curr; curr = ObjList<Player>::next(curr)) {
            curr->enable = true;
        }
    }

    Renderer::setAmbient(level.skyCol);
    Renderer::setDirectional(glm::vec3(1.0f, -1.0f, -1.0f), level.dirCol);

    if(!polyEditMode) {
        for(int i = 0; i < selectedPieces.cnt(); i++) {
            LevelPiece* piece = &level.pieces[selectedPieces[i]];
            for(int i = 0; i < piece->vertices.cnt(); i++) {
                int next = i + 1;
                if(next == piece->vertices.cnt())
                    next = 0;
                glm::vec3 a = glm::vec3(piece->vertices[i].point, -piece->frontLayer);
                glm::vec3 b = glm::vec3(piece->vertices[next].point, -piece->frontLayer);
                Renderer::drawUILine(a, b, 0.01f, uiGreen);
            }
        }
    } else {
        polyEditModeUI();
    }

}

void Editor::deselectAll() {
    selectedPieces.clear();
}

void Editor::selectPiece(int pieceIdx) {
    selectedPieces.add(pieceIdx);
}

void Editor::polyEditModeUI() {
    LevelPiece* piece = &level.pieces[selectedPieces[0]];
    float frontZ = -piece->frontLayer;
    
    const float vertCircleRadius = 0.04f;

    for(int i = 0; i < piece->vertices.cnt(); i++) {
        int i1 = (i + 1) % piece->vertices.cnt();
        glm::vec3 p0 = glm::vec3(piece->vertices[i].point, frontZ);
        glm::vec3 p1 = glm::vec3(piece->vertices[i1].point, frontZ);
        Renderer::drawUILine(p0, p1, 0.02f, uiGreen);
    }
    for(int i = 0; i < piece->vertices.cnt(); i++) {
        glm::vec3 vertPos = glm::vec3(piece->vertices[i].point, frontZ);
        if(vertSelected(i)) {
            Renderer::drawUICircle(vertPos, vertCircleRadius, uiGreen);
        } else {
            Renderer::drawUICircle(vertPos, vertCircleRadius, uiGreen, glm::vec3(1.0f), 0.7f);
        }
    }

    glm::vec3 mousePos = Renderer::screenToWorld(getMousePos(), frontZ);
    glm::vec3 mouseDelta = mousePos - prevMousePos;
    prevMousePos = mousePos;

    if(editorMouseDragging()) {
        for(int i = 0; i < selectedVerts.cnt(); i++) {
            piece->vertices[selectedVerts[i]].point += glm::vec2(mouseDelta);
        }
        level.blocks[piece->block].update(&level);
    }
    if(editorMousePressed()) {
        if(!(editorKeyDown(ImGuiKey_LeftShift) || editorKeyDown(ImGuiKey_RightShift))) {
            bool clikedSelectedVert = false;
            for(int i = 0; i < selectedVerts.cnt(); i++) {
                glm::vec3 vertPos = glm::vec3(piece->vertices[selectedVerts[i]].point, frontZ);
                if(glm::distance(vertPos, mousePos) < vertCircleRadius) {
                    clikedSelectedVert = true;    
                }
            }
            if(!clikedSelectedVert)
                selectedVerts.clear();
        }
        for(int i = 0; i < piece->vertices.cnt(); i++) {
            glm::vec3 vertPos = glm::vec3(piece->vertices[i].point, frontZ);
            if(glm::distance(vertPos, mousePos) < vertCircleRadius) {
                selectVert(i);
                break;
            }
        }
    }

    if(editorKeyPressed(ImGuiKey_Delete) || editorKeyPressed(ImGuiKey_Backspace)) {
        for(int i = selectedVerts.cnt() - 1; i >= 0; i--) {
            deleteVert(piece, selectedVerts[i]);
        }
        level.blocks[piece->block].update(&level);
    }

    // Edge split
    for(int i = 0; i < piece->vertices.cnt(); i++) {
        int i1 = (i + 1) % piece->vertices.cnt(); 
        
        glm::vec3 p0 = glm::vec3(piece->vertices[i].point, frontZ);
        glm::vec3 p1 = glm::vec3(piece->vertices[i1].point, frontZ);
        glm::vec3 pt = glm::closestPointOnLine(mousePos, p0, p1);
        if(glm::distance(pt, mousePos) < vertCircleRadius && glm::distance(p0, mousePos) > 1.5f * vertCircleRadius && glm::distance(p1, mousePos) > 1.5f * vertCircleRadius) {
            Renderer::drawUICircle(pt, vertCircleRadius, uiGreen);
            if(editorMousePressed()) {
                selectedVerts.clear();
                piece->vertices.insertAt(i + 1, makePointVertex(pt));
                selectedVerts.add(i + 1);
                break;
            }
        }
    }

}

void Editor::selectVert(int vert) {
    if(!vertSelected(vert))
        selectedVerts.add(vert);
}

bool Editor::vertSelected(int vert) {
    for(int i = 0; i < selectedVerts.cnt(); i++)
        if(selectedVerts[i] == vert)
            return true;
    return false;
}

void Editor::deleteVert(LevelPiece* piece, int vert) {
    piece->vertices.removeAt(vert);
    for(int i = 0; i < selectedVerts.cnt(); i++) {
        if(selectedVerts[i] == vert) {
            selectedVerts[i] = selectedVerts[selectedVerts.cnt() - 1];
            selectedVerts.pop();
        } else if(selectedVerts[i] > vert) {
            selectedVerts[i]--;
        }
    }
}
