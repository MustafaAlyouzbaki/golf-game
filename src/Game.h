// Game.h
#ifndef GAME_H
#define GAME_H

#include "raylib.h"

// Constants
const float GRAVITY = 0.5f;
const float DAMPING = 0.3f;
const float FRICTION = 0.95f;
const float STOP_THRESHOLD = 5.0f;
const float LAUNCH_SCALE = 0.25f;
const float BALL_RADIUS = 15.0f;
const float GRASS_HEIGHT = 200.0f;

class Game {
public:
    // Position and movement variables
    Vector2 ballPosition;
    Vector2 velocity;
    bool isRolling;
    bool isDragging;
    
    // Dragging controls
    Vector2 startDrag;
    Vector2 currentDrag;
    
    // Stats
    int shotCount;
    int scoreCount;
    
    // Constructor
    Game();
    
    // Game functions
    void Update();
    void Draw();
    void ResetGame();
    void CheckCollisions();
    void ApplyFriction();
};

#endif
