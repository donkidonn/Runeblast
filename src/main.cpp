#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <raylib.h>
#include "tileset.cpp"
using namespace std;

int main() {
    InitWindow(1664, 992, "Rune Blast");
    SetTargetFPS(60);

    // Load the tileset texture
    Texture2D tileset = LoadTexture("FieldsTileset.png"); // Your tileset
    int tilesPerRow = tileset.width / tileWidth;

    Node* tileMap = LoadTileMap("tiles.csv");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Node* current = tileMap;
        while (current != NULL) {
            tile t = current->data;

            if (t.tileID <= 0) {
                current = current->next;
                continue;
            }

            int tileID = t.tileID - 1; // Tiled usually starts IDs at 1
            int srcX = (tileID % tilesPerRow) * tileWidth;
            int srcY = (tileID / tilesPerRow) * tileHeight;

            Rectangle srcRect = { (float)srcX, (float)srcY, (float)tileWidth, (float)tileHeight };
            Vector2 position = { (float)t.screenX, (float)t.screenY };

            DrawTextureRec(tileset, srcRect, position, WHITE);

            current = current->next;
        }

        EndDrawing();
    }

    UnloadTexture(tileset);
    CloseWindow();
    return 0;
}
