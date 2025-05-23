// === tileset.h ===
#ifndef TILESET_H
#define TILESET_H

#include <string>
#include <vector>
#include "raylib.h"
using namespace std;

extern const int tileWidth;
extern const int tileHeight;

struct tile {
    int tileID;
    int gridX, gridY;
    int screenX, screenY;
};

struct Node {
    tile data;
    Node* next;
};

// DECORATION STRUCT
struct DecorObject {
    int gid;
    float x, y;
    Texture2D texture;
};

extern vector<DecorObject> decorList;

int GetTileValue(Node* head, int y, int x);
Node* LoadTileMap(const string& filename);
Node* LoadTileMapFromTMJ(const string& filename);      // stub for future
void LoadDecorationsFromTMJ(const string& filename);   // <-- new
void DrawDecorations();                                // <-- new

#endif
