#include "raylib.h"
#include "raymath.h"
#include <cmath>

// Linked list node for Arrow
struct Arrow {
    Vector2 position;
    Vector2 target;
    float speed;
    Texture2D texture;
    bool flip;
    Arrow* next;
};

// Arrow list head
Arrow* arrowHead = nullptr;

// Utility: get angle in degrees
float GetAngle(Vector2 from, Vector2 to) {
    return atan2f(to.y - from.y, to.x - from.x) * RAD2DEG;
}

// Choose arrow sprite by compass direction
Texture2D GetArrowTextureFromAngle(float angleDeg, bool &flip) {
    flip = false;
    if (angleDeg < 0) angleDeg += 360;

    if (angleDeg >= 348.75f || angleDeg < 11.25f)      return LoadTexture("1.png");   // N
    else if (angleDeg < 33.75f)  return LoadTexture("3.png");   // NbE
    else if (angleDeg < 56.25f)  return LoadTexture("4.png");   // NNE
    else if (angleDeg < 78.75f)  return LoadTexture("6.png");   // NEbN
    else if (angleDeg < 101.25f) return LoadTexture("7.png");   // NE
    else if (angleDeg < 123.75f) return LoadTexture("8.png");   // NEbE
    else if (angleDeg < 146.25f) return LoadTexture("9.png");   // ENE
    else if (angleDeg < 168.75f) return LoadTexture("11.png");  // EbN
    else if (angleDeg < 191.25f) return LoadTexture("13.png");  // E

    flip = true;

    if (angleDeg < 213.75f)      return LoadTexture("13.png");  // W
    else if (angleDeg < 236.25f) return LoadTexture("11.png");
    else if (angleDeg < 258.75f) return LoadTexture("9.png");
    else if (angleDeg < 281.25f) return LoadTexture("8.png");
    else if (angleDeg < 303.75f) return LoadTexture("7.png");
    else if (angleDeg < 326.25f) return LoadTexture("6.png");
    else if (angleDeg < 348.75f) return LoadTexture("4.png");

    return LoadTexture("1.png"); // fallback
}

// Add a new arrow to linked list
void AddArrow(Vector2 startPos, Vector2 target) {
    Arrow* newArrow = new Arrow;
    newArrow->position = startPos;
    newArrow->target = target;
    newArrow->speed = 4.0f;

    bool flip = false;
    float angleDeg = GetAngle(startPos, target);
    newArrow->texture = GetArrowTextureFromAngle(angleDeg, flip);
    newArrow->flip = flip;
    newArrow->next = arrowHead;
    arrowHead = newArrow;
}

// Update all arrows
void UpdateArrows() {
    Arrow** current = &arrowHead;
    while (*current) {
        Arrow* a = *current;
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

// Draw all arrows
void DrawArrows() {
    for (Arrow* a = arrowHead; a != nullptr; a = a->next) {
        Rectangle src = { 0, 0, (float)a->texture.width, (float)a->texture.height };
        Vector2 origin = { a->texture.width / 2.0f, a->texture.height / 2.0f };
        Rectangle dest = {
            a->position.x, a->position.y,
            a->flip ? -(float)a->texture.width : (float)a->texture.width,
            (float)a->texture.height
        };
        DrawTexturePro(a->texture, src, dest, origin, 0.0f, WHITE);
    }
}

// === MAIN PROGRAM ===
int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Compass Arrow Linked List");
    SetTargetFPS(60);

    Texture2D towerTex = LoadTexture("Upgrade2.png");
    Texture2D archerTex = LoadTexture("Up_idle.png");

    const int towerFrameWidth = 70;
    const int towerFrameHeight = 90;
    const int towerFrameCount = 4;

    const int archerFrameCount = 4;
    const int archerFrameWidth = archerTex.width / archerFrameCount;
    const int archerFrameHeight = archerTex.height;

    int currentTowerFrame = 0;
    int currentArcherFrame = 0;
    int frameCounter = 0;
    bool towerAnimDone = false;

    Vector2 towerPos = {screenWidth / 2 - towerFrameWidth / 2, screenHeight / 2};
    Vector2 archerPos;

    while (!WindowShouldClose()) {
        frameCounter++;

        // Animate tower
        if (!towerAnimDone && frameCounter % 10 == 0) {
            currentTowerFrame++;
            if (currentTowerFrame >= towerFrameCount) {
                currentTowerFrame = towerFrameCount - 1;
                towerAnimDone = true;
            }
        }

        // Animate archer
        if (towerAnimDone && frameCounter % 10 == 0) {
            currentArcherFrame = (currentArcherFrame + 1) % archerFrameCount;
        }

        // Shoot arrow
        Vector2 mouse = GetMousePosition();
        if (towerAnimDone && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 fireFrom = {
                towerPos.x + towerFrameWidth / 2.0f,
                towerPos.y - archerFrameHeight / 2.0f
            };
            AddArrow(fireFrom, mouse);
        }

        UpdateArrows();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw tower
        Rectangle towerSrc = {
            (float)(towerFrameWidth * currentTowerFrame), 190.0f,
            (float)towerFrameWidth, (float)towerFrameHeight
        };
        DrawTextureRec(towerTex, towerSrc, towerPos, WHITE);

        // Draw archer
        if (towerAnimDone) {
            Rectangle archerSrc = {
                (float)(archerFrameWidth * currentArcherFrame), 0.0f,
                (float)archerFrameWidth, (float)archerFrameHeight
            };
            int offsetY = 40;
            archerPos.x = towerPos.x + towerFrameWidth / 2 - archerFrameWidth / 2;
            archerPos.y = towerPos.y - archerTex.height + offsetY;
            DrawTextureRec(archerTex, archerSrc, archerPos, WHITE);
        }

        // Draw arrows
        DrawArrows();

        DrawText("Click to fire compass arrow", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }

    UnloadTexture(towerTex);
    UnloadTexture(archerTex);
    CloseWindow();
    return 0;
}
