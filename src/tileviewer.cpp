#include "raylib.h"
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using json = nlohmann::json;
using namespace std;

struct Tileset {
    Texture2D texture;
    int firstgid;
    int tileWidth;
    int tileHeight;
    int columns;
};

unordered_map<int, Texture2D> gidTextures; // NEW: map gid → Texture

json LoadTMJ(const char* filePath) {
    ifstream in(filePath);
    json j;
    in >> j;
    return j;
}

Texture2D LoadTilesetTexture(const std::string& imagePath) {
    if (!std::ifstream(imagePath)) {
        cout << "❌ Image not found: " << imagePath << endl;
        return LoadTextureFromImage(GenImageColor(1, 1, RED));
    }
    Image img = LoadImage(imagePath.c_str());
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

int main() {
    InitWindow(1664, 992, "TMJ Viewer with Decorations");
    SetTargetFPS(60);

    json mapData = LoadTMJ("FINALPLEASE..tmj");

    int tileWidth = mapData["tilewidth"];
    int tileHeight = mapData["tileheight"];

    vector<Tileset> tilesets;

    for (auto& ts : mapData["tilesets"]) {
        if (ts.contains("image") && !ts["image"].is_null()) {
            // Normal tileset
            Tileset tset;
            tset.firstgid = ts["firstgid"];
            tset.tileWidth = ts["tilewidth"];
            tset.tileHeight = ts["tileheight"];
            tset.columns = ts["columns"];

            string imagePath = ts["image"];
            std::replace(imagePath.begin(), imagePath.end(), '\\', '/');

            cout << "📦 Loading tileset: " << imagePath << endl;
            tset.texture = LoadTilesetTexture(imagePath);
            tilesets.push_back(tset);
        }

        // NEW: support for "collection of images"
        if (ts.contains("tiles")) {
            int firstgid = ts["firstgid"];
            for (auto& tile : ts["tiles"]) {
                if (tile.contains("image")) {
                    string imagePath = tile["image"];
                    std::replace(imagePath.begin(), imagePath.end(), '\\', '/');
                    int id = tile["id"];
                    int gid = firstgid + id;

                    cout << "📦 Loading image tile gid " << gid << ": " << imagePath << endl;
                    gidTextures[gid] = LoadTilesetTexture(imagePath);
                }
            }
        }
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw tile layers
        for (auto& layer : mapData["layers"]) {
            if (layer["type"] != "tilelayer") continue;

            const auto& data = layer["data"];
            int layerWidth = layer["width"];

            for (int i = 0; i < data.size(); i++) {
                int tileId = data[i];
                if (tileId == 0) continue;

                Tileset* currentTileset = nullptr;
                for (auto& ts : tilesets) {
                    if (tileId >= ts.firstgid) currentTileset = &ts;
                }

                if (!currentTileset) continue;

                int localId = tileId - currentTileset->firstgid;
                int sx = (localId % currentTileset->columns) * currentTileset->tileWidth;
                int sy = (localId / currentTileset->columns) * currentTileset->tileHeight;

                int dx = (i % layerWidth) * tileWidth;
                int dy = (i / layerWidth) * tileHeight;

                Rectangle src = { (float)sx, (float)sy, (float)tileWidth, (float)tileHeight };
                DrawTextureRec(currentTileset->texture, src, { (float)dx, (float)dy }, WHITE);
            }
        }

        // Draw object decorations
        for (auto& layer : mapData["layers"]) {
            if (layer["type"] != "objectgroup") continue;

            for (auto& obj : layer["objects"]) {
                if (!obj.contains("gid")) continue;

                int gid = obj["gid"];
                float x = obj["x"];
                float y = obj["y"];

                if (gidTextures.count(gid)) {
                    Texture2D tex = gidTextures[gid];
                    int w = tex.width;
                    int h = tex.height;
                    DrawTexture(tex, (int)x, (int)(y - h), WHITE);
                } else {
                    // fallback if part of a sprite-based tileset
                    Tileset* currentTileset = nullptr;
                    for (auto& ts : tilesets) {
                        if (gid >= ts.firstgid) currentTileset = &ts;
                    }
                    if (!currentTileset) continue;

                    int localId = gid - currentTileset->firstgid;
                    int sx = (localId % currentTileset->columns) * currentTileset->tileWidth;
                    int sy = (localId / currentTileset->columns) * currentTileset->tileHeight;
                    Rectangle src = { (float)sx, (float)sy, (float)tileWidth, (float)tileHeight };
                    DrawTextureRec(currentTileset->texture, src, { x, y - tileHeight }, WHITE);
                }
            }
        }

        EndDrawing();
    }

    // Cleanup
    for (auto& ts : tilesets) UnloadTexture(ts.texture);
    for (auto& [_, tex] : gidTextures) UnloadTexture(tex);

    CloseWindow();
    return 0;
}
