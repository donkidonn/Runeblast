#include <raylib.h>

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Simple Vertical Spider");
    SetTargetFPS(60);

    // Load sprite
    Texture2D spider = LoadTexture("spider.png");

    // Frame info
    const int frameWidth = 64;
    const int frameHeight = 64;

    // Position
    float posX = screenWidth / 2 - frameWidth / 2;
    float posY = 0;

    // Speed
    float speed = 100.0f;

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();
        posY += speed * delta;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw only the first frame
        Rectangle source = { 0, 0, (float)frameWidth, (float)frameHeight };
        Rectangle dest = { posX, posY, (float)frameWidth, (float)frameHeight };

        DrawTexturePro(spider, source, dest, {0, 0}, 0.0f, WHITE);

        EndDrawing();
    }

    UnloadTexture(spider);
    CloseWindow();

    return 0;
}
