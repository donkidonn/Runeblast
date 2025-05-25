// tower.h
#ifndef TOWER_H
#define TOWER_H

#include "raylib.h"
#include "pathway.h"
#include <raymath.h>

// Tower animation states
enum TowerState {
    TOWER_UPGRADING,
    TOWER_DONE
};

// Arrow linked list
struct ArrowNode {
    Vector2 position;
    Vector2 target;
    float speed;
    Texture2D texture;
    bool flip;
    ArrowNode* next;
};

// Tower + archer state
struct TowerNode {
    Vector2 position;
    int upgradeFrame;
    int frameTimer;
    TowerState state;

    int archerFrame;
    int archerTimer;

    ArrowNode* arrowHead;
    TowerNode* next;
    float attackCooldown;
    int archerFrameCount;


    Texture2D currentArcherTexture;
};

extern TowerNode* towerHead;
extern int towersBuilt;

// Core control
void SpawnTower(Vector2 position);
void UpdateTowers(TowerNode*& head, float delta);
void DrawTowers(TowerNode* head);
bool IsTowerOccupied(Vector2 position);
void RemoveTower(Vector2 position);


#endif
