#include "tileset.h"
#include <fstream>
#include <sstream>

const int tileWidth = 32;
const int tileHeight = 32;

int GetTileValue(Node* head, int y, int x) {
    Node* current = head;
    while (current) {
        if (current->data.gridX == x && current->data.gridY == y)
            return current->data.tileID;
        current = current->next;
    }
    return -1;
}

Node* LoadTileMap(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return nullptr;

    Node* head = nullptr;
    Node* tail = nullptr;
    string line;
    int gridY = 0;
 
    while (getline(file, line)) {
        stringstream ss(line);
        string storeID;
        int gridX = 0;

        while (getline(ss, storeID, ',')) {
            if (storeID.empty()) {
                gridX++;
                continue;
            }

            try {
                int tileID = stoi(storeID);
                int screenX = gridX * tileWidth;
                int screenY = gridY * tileHeight;

                tile newTile = { tileID, gridX, gridY, screenX, screenY };
                Node* newNode = new Node{ newTile, nullptr };

                if (!head) head = tail = newNode;
                else {
                    tail->next = newNode;
                    tail = newNode;
                }
            } catch (...) {}
            gridX++;
        }
        gridY++;
    }

    file.close();
    return head;
}
