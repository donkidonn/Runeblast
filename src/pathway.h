// === pathway.h ===
#ifndef PATHWAY_H
#define PATHWAY_H

#include "raylib.h"
#include "tileset.h"

struct VisitedNode {
    Vector2 tile;
    VisitedNode* next;
};

struct Enemy {
    Vector2 position;
    Vector2 currentTile;
    Vector2 previousTile;
    Vector2 targetTilePos;
    Vector2 facingDir;
    float rotation;
    float speed;
    float frameTimer;
    int currentFrame;

    VisitedNode* visitedHead = nullptr;

    int health;
    int maxHealth;
    bool isHit;
    float hitTimer;

    Enemy* next;
};

// === Spawn Queue Node ===
struct SpawnNode {
    Vector2 spawnTile;
    SpawnNode* next;
};

extern Enemy* headEnemy;
extern SpawnNode* spawnFront;
extern SpawnNode* spawnRear;

void Enqueue(Vector2 tileCoord);
bool Dequeue(Vector2& outTile);

void SpawnManager(Node* tileMap, float& spawnTimer, float spawnDelay);
void SpawnEnemy(Enemy*& head, Vector2 tileCoord, Node* tileMap);
void AddToVisited(VisitedNode*& head, Vector2 tile);
bool IsVisited(VisitedNode* head, Vector2 tile);
Vector2 GetNextTile(Vector2 currentTile, Node* tileMap, VisitedNode* visitedHead);
void UpdateEnemies(Enemy*& head, Node* tileMap, float delta);
void DamageEnemy(Enemy* enemy, int damage);

// === Hitbox Helper ===
inline Rectangle GetEnemyHitbox(const Enemy* e) {
    return Rectangle{
        e->position.x - 16,
        e->position.y - 16,
        32, 32
    };
}

#endif