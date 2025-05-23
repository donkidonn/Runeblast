#include "tileset.h"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "json.hpp"
using namespace std;
using json = nlohmann::json;

const int tileWidth = 32;
const int tileHeight = 32;
vector<DecorObject> decorList;

int GetTileValue(Node* head, int y, int x) {
    int row = 0;
    Node* current = head;
    while (current && row < y) {
        current = current->next;
        row++;
    }
    if (!current) return -1;

    tile t = current->data;
    if (t.gridY == y && t.gridX == x) return t.tileID;

    while (current) {
        tile t = current->data;
        if (t.gridY == y && t.gridX == x) return t.tileID;
        current = current->next;
    }
    return -1;
}

Node* LoadTileMap(const string& filename) {
    // Placeholder if you need CSV support later
    return nullptr;
}

Node* LoadTileMapFromTMJ(const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cout << "Failed to open TMJ: " << filename << endl;
        return nullptr;
    }

    json mapData;
    in >> mapData;

    Node* head = nullptr;
    Node* tail = nullptr;

    int mapWidth = mapData["width"];
    int mapHeight = mapData["height"];

    for (auto& layer : mapData["layers"]) {
        if (layer["type"] != "tilelayer") continue;

        const auto& data = layer["data"];
        for (int i = 0; i < data.size(); i++) {
            int tileID = data[i];
            int x = i % mapWidth;
            int y = i / mapWidth;

            tile t;
            t.tileID = tileID;
            t.gridX = x;
            t.gridY = y;
            t.screenX = x * tileWidth;
            t.screenY = y * tileHeight;

            Node* newNode = new Node{t, nullptr};
            if (!head) head = tail = newNode;
            else {
                tail->next = newNode;
                tail = newNode;
            }
        }
    }

    return head;
}

void LoadDecorationsFromTMJ(const string& filename) {
    ifstream in(filename);
    if (!in.is_open()) {
        cout << "Failed to open TMJ: " << filename << endl;
        return;
    }

    json mapData;
    in >> mapData;

    unordered_map<int, Texture2D> gidTextureMap;

    for (auto& ts : mapData["tilesets"]) {
        int firstgid = ts["firstgid"];
        if (ts.contains("tiles")) {
            for (auto& tile : ts["tiles"]) {
                if (tile.contains("image")) {
                    string path = tile["image"];
                    std::replace(path.begin(), path.end(), '\\', '/');
                    int localId = tile["id"];
                    int gid = firstgid + localId;

                    Texture2D tex = LoadTexture(path.c_str());
                    gidTextureMap[gid] = tex;
                }
            }
        }
    }

    for (auto& layer : mapData["layers"]) {
        if (layer["type"] != "objectgroup") continue;

        for (auto& obj : layer["objects"]) {
            if (!obj.contains("gid")) continue;

            int gid = obj["gid"];
            float x = obj["x"];
            float y = obj["y"];

            if (gidTextureMap.count(gid)) {
                Texture2D tex = gidTextureMap[gid];
                decorList.push_back({ gid, x, y - tex.height, tex });
            }
        }
    }

    cout << "✅ Decorations loaded: " << decorList.size() << endl;
}

void DrawDecorations() {
    for (const auto& d : decorList) {
        DrawTexture(d.texture, (int)d.x, (int)d.y, WHITE);
    }
}
