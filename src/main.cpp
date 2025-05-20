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

    /*
    float spawnTimer = 0.0f;
    float spawnDelay = 5.0f; */
    bool enemySpawned = false;

    const int spiderFrameWidth = 64;
    const int spiderFrameHeight = 64;

    while (!WindowShouldClose()) {
        /*
        spawnTimer += GetFrameTime();
        if (spawnTimer >= spawnDelay) {
            SpawnEnemy(headEnemy, { 0, 5 }, tileMap);
            spawnTimer = 0.0f;
        } */

        if (!enemySpawned) {
            SpawnEnemy(headEnemy, { 0, 5 }, tileMap);
            enemySpawned = true;
        }

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
                64, // scaled width
                64  // scaled height
            };
            DrawTexturePro(spiderTexture, srcRect, destRect, {32, 32}, 0.0f, WHITE);

            spider = spider->next;
        }

        EndDrawing();
    }

    UnloadTexture(tileset);
    UnloadTexture(spiderTexture);
    CloseWindow();
    return 0;
}
