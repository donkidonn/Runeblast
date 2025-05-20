# Runeblast
A tower defense game developed in C++ using raylib library

readme = myNotes :D

//to run the game
step 1. cd to ur game file path

step 2. run this in ucrt64 (msys)
g++ main.cpp pathway.cpp tileset.cpp -o RuneBlast -lraylib -lopengl32 -lgdi32 -lwinmm

g++ main.cpp -o game.exe -lraylib -lopengl32 -lgdi32 -lwinmm


step 3. do ./game or ./game.exe

Pathfinding algorithm (will change siguro)
1. starts at specific tile.
2. y + 1 //meaning it will move to the right
3. then when nag move na, it will check its 4 directions (up down right left)
4. when checking icheck niya if ang tileID is = 1. 
5. If nakahanap siya na ang tileID is 1, icheck niya if napuntahan niya na ba yun or not. if yes then ignore, if not then mag momove siya dun.