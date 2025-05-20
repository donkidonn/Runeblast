#ifndef PATHWAY_H
#define PATHWAY_H

#include "raylib.h"
#include "tileset.h"
#include <vector>

struct Enemy {
    Vector2 position;
    Vector2 currentTile;
    Vector2 previousTile;
    Vector2 targetTilePos;
    std::vector<Vector2> visitedTiles;

    float speed;
    float frameTimer;
    int currentFrame;

    Enemy* next;
};

extern Enemy* headEnemy;

void SpawnEnemy(Enemy*& head, Vector2 tileCoord, Node* tileMap);
Vector2 GetNextTile(Vector2 currentTile, Vector2 previousTile, Node* tileMap);
void UpdateEnemies(Enemy*& head, Node* tileMap, float delta);

#endif
