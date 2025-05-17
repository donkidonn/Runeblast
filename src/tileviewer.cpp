#include "raylib.h"
#include <string>

const int TILE_SIZE = 32;
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tileset Viewer");
    SetTargetFPS(60);

    Texture2D tileset = LoadTexture("FieldsTileset.png");
    Texture2D tileset2 = LoadTexture("FieldsTile_38.png");
    Texture2D grass = LoadTexture("grass.png");

    int tilesPerRow = tileset.width / TILE_SIZE;
    int tilesPerCol = tileset.height / TILE_SIZE;
    int totalTiles = tilesPerRow * tilesPerCol;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        //fits the png file to the window

        float scaleX = (float)WINDOW_WIDTH / tileset2.width;
        float scaleY = (float)WINDOW_HEIGHT / tileset2.height;
        DrawTextureEx(tileset2, {0, 0}, 0.0f, scaleX, WHITE); //the purpose of white is to make the image transparent
        
        float scale = 10.0f;
        Vector2 screenCenter = {
            (GetScreenWidth() - grass.width * scale) / 2,
            (GetScreenHeight() - grass.height * scale) / 2
        };

        
        DrawTextureEx(grass, screenCenter, 0.0f, scale, WHITE);



        int padding = 10;
        int tilesPerScreenRow = WINDOW_WIDTH / (TILE_SIZE + padding);
        int tileIndex = 0;

        for (int i = 0; i < totalTiles; i++) {
            int tileX = i % tilesPerRow;
            int tileY = i / tilesPerRow;

            Rectangle src = {
                (float)(tileX * TILE_SIZE),
                (float)(tileY * TILE_SIZE),
                (float)TILE_SIZE,
                (float)TILE_SIZE
            };

            int screenX = (tileIndex % tilesPerScreenRow) * (TILE_SIZE + padding);
            int screenY = (tileIndex / tilesPerScreenRow) * (TILE_SIZE + 30); // Extra for label

            DrawTextureRec(tileset, src, (Vector2){(float)screenX, (float)screenY}, WHITE);
            
            std::string label = "Tile " + std::to_string(i);
            DrawText(label.c_str(), screenX, screenY + TILE_SIZE + 2, 10, DARKGRAY);

            tileIndex++;
        }

        EndDrawing();
    }

    UnloadTexture(tileset);
    CloseWindow();
    return 0;
}
