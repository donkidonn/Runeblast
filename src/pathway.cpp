#include "raylib.h"
#include <iostream>
using namespace std;

// Tile size
const int tileWidth = 32;
const int tileHeight = 32;

// Enemy structure
struct Enemy {
    Vector2 position;         // Pixel position
    Vector2 currentTile;      // Grid position (tile X/Y)
    Vector2 previousTile;     // Last tile visited (for path checking)
    Vector2 targetTilePos;    // Pixel position of next tile

    float speed = 60.0f;      // Movement speed (pixels per second)
    float frameTimer = 0.0f;  // Animation frame timer
    int currentFrame = 0;     // Frame index

    Enemy* next;              // Linked list pointer
};

Enemy* headEnemy = nullptr;

// Spawns an enemy at a specific tile coordinate
void SpawnEnemy(Enemy*& head, Vector2 tileCoord) {
    // Convert tile to pixel position
    Vector2 pixelPos = {
        tileCoord.x * tileWidth,
        tileCoord.y * tileHeight
    };

    // Create new enemy
    Enemy* newEnemy = new Enemy;
    newEnemy->position = pixelPos;
    newEnemy->currentTile = tileCoord;
    newEnemy->previousTile = tileCoord;
    newEnemy->targetTilePos = pixelPos;
    newEnemy->next = nullptr;
    
    if (!head) {
        head = newEnemy;
    } else {
        Enemy* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newEnemy;
    }
}
