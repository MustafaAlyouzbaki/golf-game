#ifndef BALL_H
#define BALL_H

#include "raylib.h"
#include "math.h"
#include "screenSizeTransfer.h"
#include "terrain.h"

// Constants (built with standard 1280x720 res)
const float GRAVITY = 0.5f;
const float DAMPING = 0.3f;
const float FRICTION = 0.95f;
const Vector2 STOP_THRESHOLD = { 0.1f, 5.0f };
const float LAUNCH_SCALE = 0.1f;
const float BALL_RADIUS = 12.0f;
const float GRASS_HEIGHT = 160.0f; //Minimum height

class Ball
{
    private:
        //Position and movement variables
        Vector2 ballPosition;
        Vector2 velocity;
        Vector2 previousStartPosition;
        
        //Stats
        int shotCount;
    public:
        //Misc
        CLITERAL(Color) ballColor;

        //Draggin stuff
        Vector2 startDrag;
        Vector2 currentDrag;
        bool isDragging;
        bool isRolling;
        bool isStopped;
        bool showTrajectory;
        bool showHelpMenu;
        bool toggleRainbow;

        Ball();
        Ball(CLITERAL(Color) color);
        //Draws the ball on the screen
        void draw(const Camera2D& camera, const std::vector<TerrainSquare>& terrain);
        void drawDebug();
        void applyFriction();
        void updatePhysics(const std::vector<TerrainSquare>& terrain);
        void checkCollisions(const std::vector<TerrainSquare>& terrain);
        void updateLogic(); 
        void updateVelocity(Vector2 newVel);
        void drawTrajectory(const Camera2D& camera, const std::vector<TerrainSquare>& terrain);
        void drawHelpMenu(const Camera2D& camera);
        void updateColor(Color c);
        void updateColorOverTime(float deltaTime);
        //ResetGame needs to be overhauled
        Vector2 getBallPosition();
        Vector2 getVelocity();
        Vector2 calculateInitialVelocity();
};

#endif
