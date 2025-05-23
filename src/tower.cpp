// tower.cpp
#include "tower.h"
#include <cmath>
#include <raymath.h>

TowerNode* towerHead = nullptr;

Texture2D upgradeTex;
Texture2D archerTex;

const int towerUpgradeFrames = 4;
const int towerUpgradeSpeed = 10;
const int archerFrameCount = 4;
const int archerSpeed = 30;

void SpawnTower(Vector2 position) {
    TowerNode* tower = new TowerNode;
    tower->position = position;
    tower->upgradeFrame = 0;
    tower->frameTimer = 0;
    tower->state = TOWER_UPGRADING;
    tower->archerFrame = 0;
    tower->archerTimer = 0;
    tower->arrowHead = nullptr;
    tower->next = towerHead;
    towerHead = tower;
}

void UpdateArrows(ArrowNode*& head) {
    ArrowNode** current = &head;
    while (*current) {
        ArrowNode* a = *current;
        Vector2 dir = Vector2Subtract(a->target, a->position);
        float dist = Vector2Length(dir);

        if (dist < 5.0f) {
            *current = a->next;
            UnloadTexture(a->texture);
            delete a;
            continue;
        }

        dir = Vector2Normalize(dir);
        a->position = Vector2Add(a->position, Vector2Scale(dir, a->speed));
        current = &(*current)->next;
    }
}

void FireArrow(TowerNode* tower, Enemy* target) {
    if (!target) return;
    ArrowNode* arrow = new ArrowNode;
    arrow->position = tower->position;
    arrow->target = target->position;
    arrow->speed = 10.0f;

    float angle = atan2f(arrow->target.y - arrow->position.y, arrow->target.x - arrow->position.x) * RAD2DEG;
    arrow->flip = false;
    arrow->texture = LoadTexture("1.png"); // Temp: all use 1.png for now
    arrow->next = tower->arrowHead;
    tower->arrowHead = arrow;
}

void UpdateTowers(TowerNode*& head, float delta) {
    for (TowerNode* t = head; t != nullptr; t = t->next) {
        if (t->state == TOWER_UPGRADING) {
            t->frameTimer++;
            if (t->frameTimer >= towerUpgradeSpeed) {
                t->frameTimer = 0;
                t->upgradeFrame++;
                if (t->upgradeFrame >= towerUpgradeFrames) {
                    t->upgradeFrame = towerUpgradeFrames - 1;
                    t->state = TOWER_DONE;
                }
            }
        } else if (t->state == TOWER_DONE) {
            t->archerTimer++;
            if (t->archerTimer >= archerSpeed) {
                t->archerTimer = 0;
                t->archerFrame = (t->archerFrame + 1) % archerFrameCount;

                // Find and shoot nearest enemy
                Enemy* target = headEnemy;
                float minDist = 1000.0f;
                Enemy* closest = nullptr;
                while (target) {
                    float d = Vector2Distance(t->position, target->position);
                    if (d < 200.0f && d < minDist) {
                        closest = target;
                        minDist = d;
                    }
                    target = target->next;
                }
                if (closest) FireArrow(t, closest);
            }
        }
        UpdateArrows(t->arrowHead);
    }
}

void DrawTowers(TowerNode* head) {
    for (TowerNode* t = head; t != nullptr; t = t->next) {
        if (t->state == TOWER_UPGRADING) {
            Rectangle src = { (float)(t->upgradeFrame * 70), 190.0f, 70, 90 };
            DrawTextureRec(upgradeTex, src, t->position, WHITE);
        } else {
            Rectangle base = { (float)(3 * 70), 190.0f, 70, 90 };
            DrawTextureRec(upgradeTex, base, t->position, WHITE);
            Rectangle archer = { (float)(t->archerFrame * (archerTex.width / 4)), 0.0f, (float)(archerTex.width / 4), (float)archerTex.height };
            Vector2 archerPos = { t->position.x + 10, t->position.y - archerTex.height / 2 + 10 };
            DrawTextureRec(archerTex, archer, archerPos, WHITE);
        }
        for (ArrowNode* a = t->arrowHead; a != nullptr; a = a->next) {
            Rectangle src = {0, 0, (float)a->texture.width, (float)a->texture.height};
            Vector2 origin = {a->texture.width / 2.0f, a->texture.height / 2.0f};
            DrawTexturePro(a->texture, src, {a->position.x, a->position.y, (float)a->texture.width, (float)a->texture.height}, origin, 0.0f, WHITE);
        }
    }
}

bool IsTowerOccupied(Vector2 position) {
    TowerNode* current = towerHead;
    while (current) {
        if (Vector2Distance(current->position, position) < 5.0f) {
            return true;
        }
        current = current->next;
    }
    return false;
}
