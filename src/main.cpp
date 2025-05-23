#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "tileset.h"
#include "pathway.h"
#include "tower.h"

extern Texture2D upgradeTex;
extern Texture2D archerTex;
extern TowerNode* towerHead;

int main() {
    InitWindow(1664, 992, "Rune Blast");
    SetTargetFPS(60);

    Texture2D tileset = LoadTexture("FieldsTileset.png");
    Texture2D spiderTexture = LoadTexture("spider.png");
    upgradeTex = LoadTexture("Upgrade2.png");
    archerTex = LoadTexture("Up_idle.png");

    int tilesPerRow = tileset.width / tileWidth;

    Node* tileMap = LoadTileMapFromTMJ("FINALPLEASE..tmj");
    LoadDecorationsFromTMJ("FINALPLEASE..tmj");

    float spawnTimer = 0.0f;
    float spawnDelay = 3.0f;

    if (!spawnFront) {
        Enqueue({0, 5});
        Enqueue({0, 5});
        Enqueue({0, 5});
        Enqueue({0, 5});
    }

    const int spiderFrameWidth = 64;
    const int spiderFrameHeight = 64;

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        SpawnManager(tileMap, spawnTimer, spawnDelay);
        UpdateEnemies(headEnemy, tileMap, delta);
        UpdateTowers(towerHead, delta);

        for (TowerNode* t = towerHead; t != nullptr; t = t->next) {
            ArrowNode** a = &t->arrowHead;
            while (*a) {
                ArrowNode* arrow = *a;
                Enemy* e = headEnemy;
                bool hit = false;

                while (e) {
                    Rectangle hitbox = GetEnemyHitbox(e);
                    if (CheckCollisionPointRec(arrow->position, hitbox)) {
                        DamageEnemy(e, 1);
                        *a = arrow->next;
                        UnloadTexture(arrow->texture);
                        delete arrow;
                        hit = true;
                        break;
                    }
                    e = e->next;
                }
                if (!hit) a = &(*a)->next;
            }
        }

        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            Vector2 snapped = {
                (float)((int)mouse.x / tileWidth) * tileWidth,
                (float)((int)mouse.y / tileHeight) * tileHeight
            };
            SpawnTower(snapped);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        Node* current = tileMap;
        while (current) {
            tile t = current->data;
            if (t.tileID > 0) {
                int id = t.tileID - 1;
                int srcX = (id % tilesPerRow) * tileWidth;
                int srcY = (id / tilesPerRow) * tileHeight;
                Rectangle src = { (float)srcX, (float)srcY, (float)tileWidth, (float)tileHeight };
                Vector2 pos = { (float)t.screenX, (float)t.screenY };
                DrawTextureRec(tileset, src, pos, WHITE);
            }
            current = current->next;
        }

        DrawDecorations();

        Enemy* spider = headEnemy;
        while (spider) {
            Rectangle srcRect = {
                (float)(spider->currentFrame * spiderFrameWidth), 0,
                (float)spiderFrameWidth, (float)spiderFrameHeight
            };
            Rectangle destRect = {
                spider->position.x,
                spider->position.y,
                64, 64
            };
            Color tint = spider->isHit ? RED : WHITE;
            DrawTexturePro(
                spiderTexture,
                srcRect,
                destRect,
                {32, 32},
                spider->rotation,
                tint
            );

            float hpPercent = (float)spider->health / 3.0f;
            Vector2 barPos = { spider->position.x - 20, spider->position.y - 40 };
            DrawRectangleV(barPos, { 40, 5 }, GRAY);
            DrawRectangleV(barPos, { 40 * hpPercent, 5 }, RED);

            spider = spider->next;
        }

        DrawTowers(towerHead);

        EndDrawing();
    }

    UnloadTexture(tileset);
    UnloadTexture(spiderTexture);
    UnloadTexture(upgradeTex);
    UnloadTexture(archerTex);
    for (auto& d : decorList) UnloadTexture(d.texture);

    CloseWindow();
    return 0;
}
