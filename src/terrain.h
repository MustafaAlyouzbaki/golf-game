#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include "raylib.h"
#include "screenSizeTransfer.h"

class TerrainSquare {
private:
    int height;    // Height of the square
    int posX;      // X position
    int width;     // Width of the square
    int id;        // Unique identifier for the square
public:
    TerrainSquare(int height, int posX, int width, int id);

    // Getter methods
    int getHeight() const;
    int getPosX() const;
    int getWidth() const;
    int getId() const;

    // Draw method for the square
    void draw() const;
};

class Terrain {
public:
    std::vector<TerrainSquare> DrawTerrain(int difficulty);
};

#endif
