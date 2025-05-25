#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "tower.h"
#include "pathway.h"

extern Texture2D upgradeTex;
extern Texture2D archerTex;
extern Texture2D archerAttackTex;
extern int towersBuilt;
extern Sound arrowSound;

TowerNode* towerHead = nullptr;

const int towerUpgradeFrames = 4;
const int towerUpgradeSpeed = 10;
const int archerSpeed = 30;

std::string GetArrowImageFromAngle(float angle) {
    angle = fmodf(angle, 360.0f);
    if (angle < 0) angle += 360.0f;

    if (angle >= 348.75f || angle < 11.25f) return "1.png";
    else if (angle < 33.75f)  return "3.png";
    else if (angle < 56.25f)  return "4.png";
    else if (angle < 78.75f)  return "6.png";
    else if (angle < 101.25f) return "7.png";
    else if (angle < 123.75f) return "8.png";
    else if (angle < 146.25f) return "9.png";
    else if (angle < 168.75f) return "11.png";
    else if (angle < 191.25f) return "13.png";
    else return "13.png";
}

void SpawnTower(Vector2 position) {
    TowerNode* tower = new TowerNode;
    tower->position = position;
    tower->upgradeFrame = 0;
    tower->frameTimer = 0;
    tower->state = TOWER_UPGRADING;
    tower->currentArcherTexture = archerTex;
    tower->archerFrameCount = 4;
    tower->archerFrame = 0;
    tower->archerTimer = 0;
    tower->attackCooldown = 0.0f;
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
    arrow->position = Vector2Add(tower->position, {30, 0});
    arrow->target = target->position;
    arrow->speed = 10.0f;

    float angle = atan2f(arrow->target.y - arrow->position.y, arrow->target.x - arrow->position.x) * RAD2DEG;
    if (angle < 0) angle += 360.0f;

    std::string filename = GetArrowImageFromAngle(angle);
    arrow->texture = LoadTexture(filename.c_str());
    arrow->flip = false;
    arrow->next = tower->arrowHead;
    tower->arrowHead = arrow;
    PlaySound(arrowSound);
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
                t->archerFrame = (t->archerFrame + 1) % t->archerFrameCount;

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

                if (closest) {
                    FireArrow(t, closest);
                    t->currentArcherTexture = archerAttackTex;
                    t->archerFrameCount = 6;
                    t->attackCooldown = 0.3f;
                }
            }
        }

        if (t->attackCooldown > 0.0f) {
            t->attackCooldown -= delta;
            if (t->attackCooldown <= 0.0f) {
                t->currentArcherTexture = archerTex;
                t->archerFrameCount = 4;
                t->attackCooldown = 0.0f;
            }
        }

        UpdateArrows(t->arrowHead);
    }
}

void DrawTowers(TowerNode* head) {
    Vector2 mouse = GetMousePosition();
    for (TowerNode* t = head; t != nullptr; t = t->next) {
        if (t->state == TOWER_UPGRADING) {
            Rectangle src = { (float)(t->upgradeFrame * 70), 190.0f, 70, 90 };
            DrawTextureRec(upgradeTex, src, t->position, WHITE);
        } else {
            Rectangle base = { (float)(3 * 70), 190.0f, 70, 90 };
            DrawTextureRec(upgradeTex, base, t->position, WHITE);

            float frameWidth = t->currentArcherTexture.width / t->archerFrameCount;
            Rectangle archer = {
                frameWidth * t->archerFrame, 0.0f,
                frameWidth,
                (float)t->currentArcherTexture.height
            };
            Vector2 archerPos = {
                t->position.x + 10,
                t->position.y - t->currentArcherTexture.height / 2 + 10
            };
            DrawTextureRec(t->currentArcherTexture, archer, archerPos, WHITE);
        }

        // Draw range circle if mouse is hovering over the tower
        Rectangle towerRect = {t->position.x, t->position.y, 70, 90};
        if (CheckCollisionPointRec(mouse, towerRect)) {
            float rangeRadius = 150.0f;
            Vector2 towerCenter = {
                t->position.x + 35,  // center X
                t->position.y + 45   // center Y
            };
            DrawCircleLines((int)towerCenter.x, (int)towerCenter.y, rangeRadius, ColorAlpha(RED, 0.5f));
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

void RemoveTower(Vector2 position) {
    TowerNode** current = &towerHead;
    while (*current) {
        if (CheckCollisionPointRec(position, {(*current)->position.x, (*current)->position.y, 70, 90})) {
            TowerNode* toDelete = *current;
            *current = (*current)->next;

            ArrowNode* arrow = toDelete->arrowHead;
            while (arrow) {
                ArrowNode* next = arrow->next;
                UnloadTexture(arrow->texture);
                delete arrow;
                arrow = next;
            }

            delete toDelete;
            towersBuilt--;
            break;
        }
        current = &(*current)->next;
    }
}
