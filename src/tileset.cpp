#include <fstream>
#include <sstream>
#include <string>
using namespace std;

const int tileWidth = 32;
const int tileHeight = 32;

struct tile {
    int tileID;
    int gridX, gridY;
    int screenX, screenY;
};

struct Node {
    tile data;
    Node* next;
};

Node* LoadTileMap(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return nullptr;
    }

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

                if (!head) {
                    head = tail = newNode;
                } else {
                    tail->next = newNode;
                    tail = newNode;
                }
            } catch (...) {
                // Ignore invalid values
            }
            gridX++;
        }
        gridY++;
    }

    file.close();
    return head;
}
