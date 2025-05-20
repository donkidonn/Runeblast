#ifndef TILESET_H
#define TILESET_H

#include <string>
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

int GetTileValue(Node* head, int y, int x);
Node* LoadTileMap(const string& filename);

#endif
