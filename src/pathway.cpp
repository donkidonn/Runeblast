#include "pathway.h"
#include "raymath.h"
#include <cmath>
#include <iostream>

Enemy* headEnemy = nullptr;
const int totalFrames = 3;

void SpawnEnemy(Enemy*& head, Vector2 tileCoord, Node* tileMap) {
    Vector2 pixelPos = { tileCoord.x * tileWidth, tileCoord.y * tileHeight };
    using namespace std;
    cout << "Spawning at: " << tileCoord.x << "," << tileCoord.y << endl;
    cout << "Current Tile ID: " << GetTileValue(tileMap, tileCoord.y, tileCoord.x) << endl;
    cout << "Right: " << GetTileValue(tileMap, tileCoord.y, tileCoord.x + 1) << endl;
    cout << "Left: " << GetTileValue(tileMap, tileCoord.y, tileCoord.x - 1) << endl;
    cout << "Up: " << GetTileValue(tileMap, tileCoord.y - 1, tileCoord.x) << endl;
    cout << "Down: " << GetTileValue(tileMap, tileCoord.y + 1, tileCoord.x) << endl;



    Enemy* newEnemy = new Enemy;
    newEnemy->position = pixelPos;
    newEnemy->currentTile = tileCoord;
    newEnemy->visitedTiles.push_back(tileCoord);
    newEnemy->targetTilePos = pixelPos;
    newEnemy->speed = 60.0f;
    newEnemy->frameTimer = 0.0f;
    newEnemy->currentFrame = 0;
    newEnemy->next = nullptr;

    if (!head) head = newEnemy;
    else {
        Enemy* current = head;
        while (current->next) current = current->next;
        current->next = newEnemy;
    }
}

Vector2 GetNextTile(Vector2 currentTile, Node* tileMap, const std::vector<Vector2>& visited) {
    Vector2 directions[4] = {
        {1, 0},   // RIGHT
        {0, 1},   // DOWN
        {-1, 0},  // LEFT
        {0, -1}   // UP
    };

    for (int i = 0; i < 4; i++) {
        Vector2 dir = directions[i];
        Vector2 nextTile = { currentTile.x + dir.x, currentTile.y + dir.y };

        //Skip already visited tiles
        bool alreadyVisited = false;
        for (const auto& tile : visited) {
            if (tile.x == nextTile.x && tile.y == nextTile.y) {
                alreadyVisited = true;
                break;
            }
        }
        if (alreadyVisited) continue;

        //Only walk on tileID 1
        if (GetTileValue(tileMap, nextTile.y, nextTile.x) == 1) {
            return nextTile;
        }
    }

    return { -1, -1 }; // no unvisited directions
}


void UpdateEnemies(Enemy*& head, Node* tileMap, float delta) {
    Enemy* current = head;
    while (current) {
        float dx = current->targetTilePos.x - current->position.x;
        float dy = current->targetTilePos.y - current->position.y;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < 1.0f) {
            current->position = current->targetTilePos;
            current->previousTile = current->currentTile;

            cout << "Current: (" << current->currentTile.x << "," << current->currentTile.y << ")\n";
            cout << "Previous: (" << current->previousTile.x << "," << current->previousTile.y << ")\n";

            Vector2 next = GetNextTile(current->currentTile, tileMap, current->visitedTiles);

            if (next.x != -1 && next.y != -1) {
                current->visitedTiles.push_back(next);
                cout << "Next tile chosen: (" << next.x << "," << next.y << ")\n";
                current->previousTile = current->currentTile;
                current->currentTile = next;
                current->targetTilePos = {
                    current->currentTile.x * tileWidth,
                    current->currentTile.y * tileHeight
                };
            } else {
                cout << "No more valid tiles to move.\n";
                // Optional: handle dead end here
            }

        } else {
            Vector2 direction = Vector2Normalize(Vector2Subtract(current->targetTilePos, current->position));
            current->position = Vector2Add(current->position, Vector2Scale(direction, current->speed * delta));

            current->frameTimer += delta;
            if (current->frameTimer >= 0.15f) {
                current->frameTimer = 0.0f;
                current->currentFrame = (current->currentFrame + 1) % totalFrames;
            }
        }

        current = current->next;
    }
}
