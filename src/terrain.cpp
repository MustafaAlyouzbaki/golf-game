#include <iostream>
#include "terrain.h"

// Constructor
TerrainSquare::TerrainSquare(int h, int X, int w, int i) 
    : height(h), posX(X), width(w), id(i) {}

// Getter methods
int TerrainSquare::getHeight() const { return height; }
int TerrainSquare::getPosX() const { return posX; }
int TerrainSquare::getWidth() const { return width; }
int TerrainSquare::getId() const { return id; }

std::vector<TerrainSquare> Terrain::DrawTerrain(int difficulty) {
    int levelSize = 0;
    switch (difficulty) {
        case 0: levelSize = 10; break;  // Easy
        case 1: levelSize = 20; break;  // Medium
        default: levelSize = 30; break; // Hard
    }

    std::vector<TerrainSquare> terrain;

    // Ball's start position and hole position
    int offset = 50;
    int ballStartX = 12 + 80 + offset;  // Ball's starting position
    int holeX = sst::baseX - 100 - offset;      // Hole's position (given as sst::baseX - 100)

    // Calculate the total distance between the ball's start and the hole
    int totalDistance = holeX - ballStartX;

    // Calculate the width of each terrain segment (evenly distributed)
    int segmentWidth = totalDistance / levelSize;  // Integer width of each segment

    // Handle any remainder by distributing it across the last segment
    int remainder = totalDistance % levelSize;  // This is the leftover distance

    srand(static_cast<unsigned>(time(0)));  // Seed the random generator

    // Generate terrain squares
    for (int i = 0; i < levelSize; i++) {
        int randomHeight = rand() % 250 + 20;  // Random height for each segment

        // If it's the last segment, add the remainder to its width
        int currentSegmentWidth = segmentWidth;
        if (i == levelSize - 1) {
            currentSegmentWidth += remainder;  // Add the remaining distance to the last segment
        }

        // Add a TerrainSquare, adjusting its x position
        terrain.emplace_back(randomHeight, ballStartX + i * segmentWidth, currentSegmentWidth, i + 1);
    }

    return terrain;
}