#include <raylib.h>
#include "tileset.h"
#include "pathway.h"
#include <iostream>
using namespace std;

int main() {
    InitWindow(1664, 992, "Rune Blast");
    SetTargetFPS(60);

    Texture2D tileset = LoadTexture("FieldsTileset.png");
    Texture2D spiderTexture = LoadTexture("spider.png");

    int tilesPerRow = tileset.width / tileWidth;
    Node* tileMap = LoadTileMap("tiles.csv");

    float spawnTimer = 0.0f;
    float spawnDelay = 3.0f;

    // Queue 4 enemies
    if (!spawnFront) {
        Enqueue({0, 5});
        Enqueue({0, 5});
        Enqueue({0, 5});
        Enqueue({0, 5});
    }

    const int spiderFrameWidth = 64;
    const int spiderFrameHeight = 64;

    while (!WindowShouldClose()) {
        SpawnManager(tileMap, spawnTimer, spawnDelay);
        UpdateEnemies(headEnemy, tileMap, GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // draw tiles
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

        // draw enemies
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

            // health bar
            float hpPercent = (float)spider->health / 3.0f;
            Vector2 barPos = { spider->position.x - 20, spider->position.y - 40 };
            DrawRectangleV(barPos, { 40, 5 }, GRAY);
            DrawRectangleV(barPos, { 40 * hpPercent, 5 }, RED);

            spider = spider->next;
        }

        EndDrawing();
    }

    UnloadTexture(tileset);
    UnloadTexture(spiderTexture);
    CloseWindow();
    return 0;
}
