#include <raylib.h>
#include <raymath.h>
#include <iostream>
#include "tileset.h"
#include "pathway.h"
#include "tower.h"

Texture2D upgradeTex = {0};
Texture2D archerTex = {0};
Texture2D archerAttackTex = {0};
extern TowerNode* towerHead;
Sound arrowSound;

//main menu
bool gameStarted = false;
Texture2D logoTex;
Texture2D startBtnTex;
Texture2D instrBtnTex;
Texture2D backgroundTex;

//instruction menu
Texture2D instruction1Tex;
Texture2D instruction2Tex;
Texture2D nextBtnTex;
Texture2D backBtnTex;
int currentInstructionPage = 1;
bool showInstructions = false;

//information in main game
int baseHealth = 8;
int maxBaseHealth = 8;
int enemiesKilled = 0;
int towersBuilt = 0;
const int maxTowers = 5;

// Game state management
enum GameState {
    STATE_PLAYING,
    STATE_WIN,
    STATE_LOSE
};

Rectangle restartBtnRect = { 650, 620, 370, 80 };   //
Rectangle menuBtnRect = { 650, 720, 370, 80 };      // x, y, width, height

GameState gameState = STATE_PLAYING;

Texture2D winTex;
Texture2D defeatTex;


bool IsTowerSpot(Vector2 click, Vector2* snappedOut) {
    for (auto& d : decorList) {
        if (d.gid == 155) {
            Rectangle r = { d.x, d.y, (float)d.texture.width, (float)d.texture.height };
            if (CheckCollisionPointRec(click, r)) {
                *snappedOut = {
                    d.x + d.texture.width / 2.0f - 36,
                    d.y + d.texture.height / 2.0f - 4
                };
                return true;
            }
        }
    }
    return false;
}

int main() {
    InitWindow(1664, 992, "Rune Blast");
    SetTargetFPS(60);
    InitAudioDevice();

    Music bgMusic = LoadMusicStream("john_doe.wav");
    arrowSound = LoadSound("arrow.mp3");
    PlayMusicStream(bgMusic);

    Texture2D tileset = LoadTexture("FieldsTileset.png");
    Texture2D spiderTexture = LoadTexture("spider.png");
    Texture2D leafbugTexture = LoadTexture("Leafbug.png");
    Texture2D crabTexture = LoadTexture("Magma Crab.png");
    upgradeTex = LoadTexture("Upgrade2.png");
    archerTex = LoadTexture("Up_idle.png");
    archerAttackTex = LoadTexture("Up_Attack.png");
    Texture2D hammerCursor = LoadTexture("Hammer2.png");
    instruction1Tex = LoadTexture("instructioninfo1.png");
    instruction2Tex = LoadTexture("instructioninfo2.png");
    nextBtnTex = LoadTexture("nextButton.png");
    backBtnTex = LoadTexture("back.png");
    winTex = LoadTexture("win.png");
    defeatTex = LoadTexture("defeat.png");


    // Menu assets
    logoTex = LoadTexture("RuneBlast.png");
    startBtnTex = LoadTexture("startgame.png");
    instrBtnTex = LoadTexture("instructions.png");
    backgroundTex = LoadTexture("backgroundMainPage.png");

    int tilesPerRow = tileset.width / tileWidth;

    Node* tileMap = LoadTileMapFromTMJ("FINALPLEASE..tmj");
    LoadDecorationsFromTMJ("FINALPLEASE..tmj");

    float spawnTimer = 0.0f;
    float spawnDelay = 3.0f;

    if (!spawnFront) { //enemy spawn (hardcoded)
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, LEAFBUG);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, SPIDER);
        Enqueue({0, 5}, MAGMACRAB);
        Enqueue({0, 5}, MAGMACRAB);
        Enqueue({0, 5}, MAGMACRAB);
        Enqueue({0, 5}, MAGMACRAB);
        Enqueue({0, 5}, MAGMACRAB);
        Enqueue({0, 5}, MAGMACRAB);
        Enqueue({0, 5}, MAGMACRAB);
        Enqueue({0, 5}, MAGMACRAB);
        Enqueue({0, 5}, MAGMACRAB);
    }

    while (!WindowShouldClose()) {
        UpdateMusicStream(bgMusic);
        float delta = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        // === Main Menu / Instruction Pages ===
        if (!gameStarted) {
            BeginDrawing();
            ClearBackground((Color){130, 148, 71, 255});

            if (showInstructions) {
                BeginDrawing();
                ClearBackground(RAYWHITE);

                // === Draw full-screen-scaled instruction image ===
                Texture2D currentPage = (currentInstructionPage == 1) ? instruction1Tex : instruction2Tex;
                Rectangle src = { 0, 0, (float)currentPage.width, (float)currentPage.height };
                Rectangle dst = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
                Vector2 origin = { 0, 0 };
                DrawTexturePro(currentPage, src, dst, origin, 0.0f, WHITE);

                // === Draw buttons with adjustable scaling & position ===
                float backbtnScale = 0.2f;
                float nextbtnScale = 0.15f;

                // Customize positions here 
                Vector2 nextBtnPos = { 1300, 800 };  // adjust right button position (x = left to right, y = top to bottom)
                Vector2 backBtnPos = { 30, 800 };   // adjust left button position

                Rectangle nextBtnRect = {
                    nextBtnPos.x,
                    nextBtnPos.y,
                    nextBtnTex.width * nextbtnScale,
                    nextBtnTex.height * nextbtnScale
                };
                Rectangle backBtnRect = {
                    backBtnPos.x,
                    backBtnPos.y,
                    backBtnTex.width * backbtnScale,
                    backBtnTex.height * backbtnScale
                };

                // DrawTextureEx(nextBtnTex, nextBtnPos, 0.0f, nextbtnScale, WHITE);
                DrawTextureEx(backBtnTex, backBtnPos, 0.0f, backbtnScale, WHITE);

                // === Handle button clicks ===
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (CheckCollisionPointRec(mouse, nextBtnRect) && currentInstructionPage == 1) {
                        currentInstructionPage = 2;
                    } else if (CheckCollisionPointRec(mouse, backBtnRect)) {
                        if (currentInstructionPage == 2)
                            currentInstructionPage = 1;
                        else
                            showInstructions = false; // go back to main menu
                    }
                }

                EndDrawing();
                continue;
            }

            // === Regular Main Menu ===
            // background
            Rectangle source = { 0, 0, (float)backgroundTex.width, (float)backgroundTex.height };
            Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
            Vector2 origin = { 0, 0 };
            DrawTexturePro(backgroundTex, source, dest, origin, 0.0f, WHITE);

            // logo
            DrawTexture(logoTex, 100, -250, WHITE);

            // button scales
            float startScale = 0.3f;
            float instrScale = 0.35f;

            // start button
            Rectangle startRect = {
                600,
                400,
                startBtnTex.width * startScale,
                startBtnTex.height * startScale
            };
            DrawTextureEx(startBtnTex, { startRect.x, startRect.y }, 0.0f, startScale, WHITE);

            // instructions button
            Rectangle instrRect = {
                650,
                600,
                instrBtnTex.width * instrScale,
                instrBtnTex.height * instrScale
            };
            DrawTextureEx(instrBtnTex, { instrRect.x, instrRect.y }, 0.0f, instrScale, WHITE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, startRect)) {
                    gameStarted = true;
                }
                if (CheckCollisionPointRec(mouse, instrRect)) {
                    showInstructions = true;
                    currentInstructionPage = 1;
                }
            }

            EndDrawing();
            continue;
        }

        // === Game Logic ===

        // Game end logic
        if (gameState == STATE_PLAYING) {
            // Game end check
            if (baseHealth <= 0) {
                gameState = STATE_LOSE;
            } else if (enemiesKilled >= 27) { //hardcoded win condition
                gameState = STATE_WIN;
            }

            SpawnManager(tileMap, spawnTimer, spawnDelay);
            UpdateEnemies(headEnemy, tileMap, delta);
            UpdateTowers(towerHead, delta);
        }

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
            Vector2 snapped = {};
            if (IsTowerSpot(mouse, &snapped) && !IsTowerOccupied(snapped)) {
                if (towersBuilt < maxTowers) {
                    SpawnTower(snapped);
                    towersBuilt++;
                } else {
                    std::cout << "Max towers reached!\n";
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            RemoveTower(mouse);
        }

        bool hoveringTower = false;
        for (TowerNode* t = towerHead; t != nullptr; t = t->next) {
            Rectangle towerRect = {t->position.x, t->position.y, 70, 90};
            if (CheckCollisionPointRec(mouse, towerRect)) {
                hoveringTower = true;
                break;
            }
        }
        if (!hoveringTower) {
            for (auto& d : decorList) {
                if (d.gid == 155) {
                    Rectangle rect = {d.x, d.y, (float)d.texture.width, (float)d.texture.height};
                    if (CheckCollisionPointRec(mouse, rect)) {
                        hoveringTower = true;
                        break;
                    }
                }
            }
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
                Rectangle src = {(float)srcX, (float)srcY, (float)tileWidth, (float)tileHeight};
                Vector2 pos = {(float)t.screenX, (float)t.screenY};
                DrawTextureRec(tileset, src, pos, WHITE);
            }
            current = current->next;
        }

        DrawDecorations();

        Enemy* spider = headEnemy;
        while (spider) {
            Texture2D enemyTex = spiderTexture;
            int frameW = 64, frameH = 64;
            switch (spider->type) {
                case SPIDER: enemyTex = spiderTexture; break;
                case LEAFBUG: enemyTex = leafbugTexture; break;
                case MAGMACRAB: enemyTex = crabTexture; break;
            }

            Rectangle srcRect = {(float)(spider->currentFrame * frameW), 0, (float)frameW, (float)frameH};
            Rectangle destRect = {spider->position.x, spider->position.y, 64, 64};
            Color tint = spider->isHit ? RED : WHITE;
            DrawTexturePro(enemyTex, srcRect, destRect, {32, 32}, spider->rotation, tint);

            float hpPercent = (float)spider->health / spider->maxHealth;
            Vector2 barPos = {spider->position.x - 20, spider->position.y - 40};
            DrawRectangleV(barPos, {40, 5}, GRAY);
            DrawRectangleV(barPos, {40 * hpPercent, 5}, RED);

            spider = spider->next;
        }

        DrawTowers(towerHead);

        // HUD (bottom right corner)
	Color greenBg = (Color){130, 148, 71, 255};
        DrawRectangle(100, 880, 930, 80, greenBg);
        float healthPercent = (float)baseHealth / maxBaseHealth;

        DrawText("Base HP", 130, 915, 20, DARKGRAY);
        DrawRectangle(260, 915, 300, 20, GRAY);
        DrawRectangle(260, 915, (int)(300 * healthPercent), 20, RED);
        DrawText(TextFormat("Enemies Killed: %d", enemiesKilled), 600, 915, 20, DARKGRAY);
        DrawText(TextFormat("Towers Built: %d/%d", towersBuilt, maxTowers), 820, 915, 20, DARKGRAY);

        if (hoveringTower) {
            HideCursor();
            DrawTexture(hammerCursor, mouse.x, mouse.y, WHITE);
        } else {
            ShowCursor();
        }

        // === Show End Game Result ===
        if (gameState == STATE_WIN) {
            Rectangle src = { 0, 0, (float)winTex.width, (float)winTex.height };
            Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
            DrawTexturePro(winTex, src, dest, {0, 0}, 0.0f, WHITE);
        }

        if (gameState == STATE_LOSE) {
            Rectangle src = { 0, 0, (float)defeatTex.width, (float)defeatTex.height };
            Rectangle dest = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
            DrawTexturePro(defeatTex, src, dest, {0, 0}, 0.0f, WHITE);
        }

        // Hitboxes for Restart and Main Menu (on win or lose screen)
        if (gameState == STATE_WIN || gameState == STATE_LOSE) {
            // Debug visualization
            //DrawRectangleLinesEx(restartBtnRect, 2, RED);
            //DrawRectangleLinesEx(menuBtnRect, 2, BLUE);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mouse, restartBtnRect)) {
                    // RESET GAME STATE
                    baseHealth = maxBaseHealth;
                    enemiesKilled = 0;
                    towersBuilt = 0;
                    headEnemy = nullptr;
                    towerHead = nullptr;
                    spawnFront = nullptr;
                    spawnRear = nullptr;

                    // Re-enqueue enemies
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, LEAFBUG);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, SPIDER);
                    Enqueue({0, 5}, MAGMACRAB);
                    Enqueue({0, 5}, MAGMACRAB);
                    Enqueue({0, 5}, MAGMACRAB);
                    Enqueue({0, 5}, MAGMACRAB);
                    Enqueue({0, 5}, MAGMACRAB);
                    Enqueue({0, 5}, MAGMACRAB);
                    Enqueue({0, 5}, MAGMACRAB);
                    Enqueue({0, 5}, MAGMACRAB);
                    Enqueue({0, 5}, MAGMACRAB);

                    gameState = STATE_PLAYING;
                }

                if (CheckCollisionPointRec(mouse, menuBtnRect)) {
                    gameStarted = false;
                    showInstructions = false;
                    baseHealth = maxBaseHealth;
                    enemiesKilled = 0;
                    towersBuilt = 0;
                    headEnemy = nullptr;
                    towerHead = nullptr;
                    spawnFront = nullptr;
                    spawnRear = nullptr;
                    gameState = STATE_PLAYING;
                }
            }
        }


        EndDrawing();
    }

    UnloadTexture(tileset);
    UnloadTexture(spiderTexture);
    UnloadTexture(upgradeTex);
    UnloadTexture(archerTex);
    UnloadTexture(hammerCursor);
    UnloadSound(arrowSound);
    UnloadTexture(leafbugTexture);
    UnloadTexture(crabTexture);
    UnloadTexture(logoTex);
    UnloadTexture(startBtnTex);
    UnloadTexture(instrBtnTex);
    UnloadTexture(instruction1Tex);
    UnloadTexture(instruction2Tex);
    UnloadTexture(nextBtnTex);
    UnloadTexture(backBtnTex);
    UnloadTexture(winTex);
    UnloadTexture(defeatTex);

    for (auto& d : decorList) UnloadTexture(d.texture);
    CloseWindow();
    return 0;
}