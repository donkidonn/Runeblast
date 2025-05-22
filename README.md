#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Tower + Archer Stack Test");
    SetTargetFPS(10);  // Set to 10 FPS for clear animation

    // Load sprites
    Texture2D towerTex = LoadTexture("Upgrade2.png"); // new tower sprite
    Texture2D archerTex = LoadTexture("Up_idle.png"); // your archer file

    // Tower sprite frame details
    const int towerFrameWidth = 32;
    const int towerFrameHeight = 70;
    const int towerFrameCount = 4;

    int currentFrame = 0;
    int frameCounter = 0;
    bool towerAnimDone = false;

    Vector2 towerPos = {screenWidth / 2 - towerFrameWidth / 2, screenHeight / 2};
    Vector2 archerPos;

    while (!WindowShouldClose()) {
        frameCounter++;

        // Animate the tower
        if (!towerAnimDone && frameCounter % 10 == 0) {
            currentFrame++;
            if (currentFrame >= towerFrameCount) {
                currentFrame = towerFrameCount - 1;
                towerAnimDone = true;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw tower frame
        Rectangle towerSrc = { (float)(towerFrameWidth * currentFrame), 0.0f, (float)towerFrameWidth, (float)towerFrameHeight };
        DrawTextureRec(towerTex, towerSrc, towerPos, WHITE);

        // Draw archer after tower is done
        if (towerAnimDone) {
            // Adjust offsetY if needed to fit nicely
            int offsetY = 10;
            archerPos.x = towerPos.x + towerFrameWidth / 2 - archerTex.width / 2;
            archerPos.y = towerPos.y - archerTex.height + offsetY;
            DrawTexture(archerTex, archerPos.x, archerPos.y, WHITE);
        }

        DrawText("Tower anim, then archer appears on top", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    UnloadTexture(towerTex);
    UnloadTexture(archerTex);
    CloseWindow();

    return 0;
}
