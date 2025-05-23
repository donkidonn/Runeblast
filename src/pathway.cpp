// === pathway.cpp ===
#include "pathway.h"
#include "raymath.h"
#include <cmath>
#include <iostream>

Enemy* headEnemy = nullptr;
SpawnNode* spawnFront = nullptr;
SpawnNode* spawnRear = nullptr;
const int totalFrames = 3;

void Enqueue(Vector2 tileCoord) {
    SpawnNode* newNode = new SpawnNode{tileCoord, nullptr};
    if (!spawnRear) {
        spawnFront = spawnRear = newNode;
    } else {
        spawnRear->next = newNode;
        spawnRear = newNode;
    }
}

bool Dequeue(Vector2& outTile) {
    if (!spawnFront) return false;
    outTile = spawnFront->spawnTile;
    SpawnNode* temp = spawnFront;
    spawnFront = spawnFront->next;
    if (!spawnFront) spawnRear = nullptr;
    delete temp;
    return true;
}

void SpawnManager(Node* tileMap, float& spawnTimer, float spawnDelay) {
    spawnTimer += GetFrameTime();
    if (spawnTimer >= spawnDelay) {
        Vector2 spawnTile;
        if (Dequeue(spawnTile)) {
            SpawnEnemy(headEnemy, spawnTile, tileMap);
        }
        spawnTimer = 0.0f;
    }
}

void AddToVisited(VisitedNode*& head, Vector2 tile) {
    VisitedNode* newNode = new VisitedNode{ tile, nullptr };
    if (!head) {
        head = newNode;
    } else {
        VisitedNode* current = head;
        while (current->next) current = current->next;
        current->next = newNode;
    }
}

bool IsVisited(VisitedNode* head, Vector2 tile) {
    while (head) {
        if (head->tile.x == tile.x && head->tile.y == tile.y) return true;
        head = head->next;
    }
    return false;
}

void SpawnEnemy(Enemy*& head, Vector2 tileCoord, Node* tileMap) {
    Vector2 pixelPos = { tileCoord.x * tileWidth, tileCoord.y * tileHeight };
    std::cout << "Spawning at: " << tileCoord.x << "," << tileCoord.y << std::endl;

    Enemy* newEnemy = new Enemy;
    newEnemy->position = pixelPos;
    newEnemy->currentTile = tileCoord;
    newEnemy->previousTile = tileCoord;
    newEnemy->facingDir = {1, 0};
    newEnemy->rotation = 0.0f;
    newEnemy->visitedHead = nullptr;
    newEnemy->health = 50;
    newEnemy->maxHealth = 50;
    newEnemy->isHit = false;
    newEnemy->hitTimer = 0.0f;
    AddToVisited(newEnemy->visitedHead, tileCoord);
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

Vector2 GetNextTile(Vector2 currentTile, Node* tileMap, VisitedNode* visitedHead) {
    Vector2 directions[4] = {
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}
    };

    for (int i = 0; i < 4; i++) {
        Vector2 dir = directions[i];
        Vector2 nextTile = { currentTile.x + dir.x, currentTile.y + dir.y };
        if (IsVisited(visitedHead, nextTile)) continue;
        if (GetTileValue(tileMap, nextTile.y, nextTile.x) == 1) {
            return nextTile;
        }
    }
    return { -1, -1 };
}

void RemoveEnemy(Enemy*& head, Enemy* toRemove) {
    if (!head || !toRemove) return;
    if (head == toRemove) {
        head = head->next;
        delete toRemove;
        return;
    }
    Enemy* prev = head;
    while (prev->next && prev->next != toRemove) prev = prev->next;
    if (prev->next) {
        prev->next = toRemove->next;
        delete toRemove;
    }
}

void UpdateEnemies(Enemy*& head, Node* tileMap, float delta) {
    Enemy* current = head;
    Enemy* prev = nullptr;
    while (current) {
        float dx = current->targetTilePos.x - current->position.x;
        float dy = current->targetTilePos.y - current->position.y;
        float distance = sqrtf(dx * dx + dy * dy);

        if (distance < 1.0f) {
            current->position = current->targetTilePos;
            Vector2 next = GetNextTile(current->currentTile, tileMap, current->visitedHead);

            if (next.x != -1 && next.y != -1) {
                AddToVisited(current->visitedHead, next);
                current->previousTile = current->currentTile;
                current->currentTile = next;
                current->targetTilePos = {
                    current->currentTile.x * tileWidth,
                    current->currentTile.y * tileHeight
                };
            } else {
                std::cout << "Enemy reached the end or is stuck.\n";
                Enemy* toDelete = current;
                current = current->next;
                RemoveEnemy(head, toDelete);
                continue;
            }
        } else {
            Vector2 direction = Vector2Normalize(Vector2Subtract(current->targetTilePos, current->position));
            current->position = Vector2Add(current->position, Vector2Scale(direction, current->speed * delta));
            current->facingDir = direction;
            current->rotation = atan2f(direction.y, direction.x) * RAD2DEG + 270.0f;
            current->frameTimer += delta;
            if (current->frameTimer >= 0.15f) {
                current->frameTimer = 0.0f;
                current->currentFrame = (current->currentFrame + 1) % totalFrames;
            }
        }

        // Handle hit timer
        if (current->isHit) {
            current->hitTimer -= delta;
            if (current->hitTimer <= 0.0f) current->isHit = false;
        }

        // Check health
        if (current->health <= 0) {
            std::cout << "Enemy died.\n";
            Enemy* toDelete = current;
            current = current->next;
            RemoveEnemy(head, toDelete);
            continue;
        }

        prev = current;
        current = current->next;
    }
}

void DamageEnemy(Enemy* enemy, int damage) {
    if (!enemy) return;
    enemy->health -= damage;
    enemy->isHit = true;
    enemy->hitTimer = 0.2f;
}
