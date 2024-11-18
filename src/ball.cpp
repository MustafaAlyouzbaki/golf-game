// Game.cpp
#include "ball.h"

Ball::Ball()
{
    ballPosition = {BALL_RADIUS + 80, sst::baseY - BALL_RADIUS - GRASS_HEIGHT };
    previousStartPosition = ballPosition;
    velocity = { 0, 0 };
    isRolling = false;
    isDragging = false;
    isStopped = true;
    showTrajectory = false;
    showHelpMenu = true;
    toggleRainbow = false;
    shotCount = 0;
    ballColor = WHITE;
}

Ball::Ball(CLITERAL(Color) color)
{
    ballPosition = {BALL_RADIUS + 80, sst::baseY - BALL_RADIUS - GRASS_HEIGHT };
    previousStartPosition = ballPosition;
    velocity = { 0, 0 };
    isRolling = false;
    isDragging = false;
    isStopped = true;
    showTrajectory = false;
    showHelpMenu = true;
    toggleRainbow = false;
    shotCount = 0;
    ballColor = color;
}

void Ball::draw(const Camera2D& camera, const std::vector<TerrainSquare>& terrain)
{
    // Draw the trajectory if dragging and the toggle is true
    if (showTrajectory) {
        drawTrajectory(camera, terrain);
    }

    // If help menu is visible, draw the help instructions
    if (showHelpMenu) {
        drawHelpMenu(camera);
    }

    // If toggled rainbow color
    if (toggleRainbow) {
        float deltaTime = GetFrameTime();
        updateColorOverTime(deltaTime);
    }

    // Draw ball
    DrawCircleV({sst::cxf(ballPosition.x), sst::cyf(ballPosition.y)}, sst::cx(BALL_RADIUS), ballColor);
    DrawCircleLines(sst::cx(ballPosition.x), sst::cy(ballPosition.y), sst::cx(BALL_RADIUS), BLACK);

    // Draw drag line
    if (isDragging) {
        DrawLineV(startDrag, currentDrag, GRAY);
    }

    /* --- Debug Information ---
    // Draw minimum height terrain lines
    int font = 30;
    DrawRectangleLines(sst::cx(0), sst::cyf(sst::baseY - GRASS_HEIGHT), sst::cx(sst::baseX), sst::cyf(GRASS_HEIGHT), BLACK);

    // Display shot count
    DrawText(TextFormat("Shots: %d", shotCount), sst::cx(0), sst::cy(sst::baseY - 100), sst::cx(font), BLACK);

    // Display the rolling status
    DrawText(TextFormat("isRolling: %i", static_cast<int>(isRolling)), sst::cx(0), sst::cy(sst::baseY - 50), sst::cx(font), BLACK);

    // Display the current velocity of the ball (optional, for debug purposes)
    DrawText(TextFormat("Velocity: %.2f, %.2f", velocity.x, velocity.y), sst::cx(0), sst::cy(sst::baseY - 150), sst::cx(font), BLACK);
    
    // Optionally display the current drag start and end positions
    if (isDragging) {
        DrawText(TextFormat("Drag Start: (%.2f, %.2f)", startDrag.x, startDrag.y), sst::cx(0), sst::cy(sst::baseY - 200), sst::cx(font), BLACK);
        DrawText(TextFormat("Current Drag: (%.2f, %.2f)", currentDrag.x, currentDrag.y), sst::cx(0), sst::cy(sst::baseY - 250), sst::cx(font), BLACK);
    }
    */
}

Vector2 Ball::calculateInitialVelocity()
{
    // Calculate the direction of the drag line (from startDrag to currentDrag)
    Vector2 direction = { startDrag.x - currentDrag.x, startDrag.y - currentDrag.y };

    // Calculate the length of the drag line (this will be the speed of the shot)
    float speed = sqrt(direction.x * direction.x + direction.y * direction.y);

    // Normalize the direction vector to ensure the velocity direction is correct
    if (speed > 0)
    {
        direction.x /= speed;
        direction.y /= speed;
    }

    // Scale the direction to the desired velocity (using the drag distance to scale speed)
    float velocityMagnitude = speed * LAUNCH_SCALE;

    // Return the initial velocity vector
    return { direction.x * velocityMagnitude, direction.y * velocityMagnitude };
}


void Ball::drawTrajectory(const Camera2D& camera, const std::vector<TerrainSquare>& terrain)
{
    // Only draw the trajectory if the ball is being dragged
    if (isDragging)
    {
        // Calculate the initial velocity based on the drag line
        Vector2 initialVelocity = calculateInitialVelocity();

        // Simulate the trajectory
        Vector2 trajectoryPosition = startDrag; // Starting position
        Vector2 velocity = initialVelocity;

        // Store the previous position to draw the trajectory line
        Vector2 previousPosition = trajectoryPosition;

        const float timeStep = 0.05f; // Smaller time step for accurate simulation
        const float groundLevel = sst::baseY - GRASS_HEIGHT - BALL_RADIUS;

        // Simulate until the ball stops or reaches a maximum simulation duration
        while (fabs(velocity.x) > STOP_THRESHOLD.x || fabs(velocity.y) > STOP_THRESHOLD.y)
        {
            // Apply gravity
            velocity.y += GRAVITY * timeStep;

            // Update position based on velocity
            trajectoryPosition.x += velocity.x * timeStep;
            trajectoryPosition.y += velocity.y * timeStep;

            // Handle collisions with ground, walls, and terrain
            if (trajectoryPosition.y >= groundLevel) 
            {
                trajectoryPosition.y = groundLevel;
                if (fabs(velocity.y) < STOP_THRESHOLD.y)
                {
                    velocity.y = 0;
                }
                else
                {
                    velocity.y *= -DAMPING;
                }

                // Simulate rolling friction
                if (fabs(velocity.x) < STOP_THRESHOLD.x)
                {
                    velocity.x = 0;
                }
                else
                {
                    velocity.x *= FRICTION;
                }
            }

            // Handle collisions with the left and right walls
            if (trajectoryPosition.x <= BALL_RADIUS) 
            {
                trajectoryPosition.x = BALL_RADIUS;
                velocity.x *= -DAMPING;
            } 
            else if (trajectoryPosition.x >= sst::baseX - BALL_RADIUS) 
            {
                trajectoryPosition.x = sst::baseX - BALL_RADIUS;
                velocity.x *= -DAMPING;
            }

            // Handle collisions with the top wall
            if (trajectoryPosition.y <= BALL_RADIUS) 
            {
                trajectoryPosition.y = BALL_RADIUS;
                velocity.y *= -DAMPING;
            }

            // Handle collisions with terrain
            for (const TerrainSquare& square : terrain)
            {
                int squareX = square.getPosX();
                int squareWidth = square.getWidth();
                int squareY = sst::baseY - GRASS_HEIGHT - square.getHeight();
                int squareHeight = square.getHeight();

                bool withinXBounds = (trajectoryPosition.x + BALL_RADIUS > squareX) && (trajectoryPosition.x - BALL_RADIUS < squareX + squareWidth);
                bool withinYBounds = (trajectoryPosition.y + BALL_RADIUS > squareY) && (trajectoryPosition.y - BALL_RADIUS < squareY + squareHeight);

                if (withinXBounds && withinYBounds)
                {
                    float overlapTop = squareY - (trajectoryPosition.y + BALL_RADIUS);
                    float overlapBottom = (trajectoryPosition.y - BALL_RADIUS) - (squareY + squareHeight);
                    float overlapLeft = squareX - (trajectoryPosition.x + BALL_RADIUS);
                    float overlapRight = (trajectoryPosition.x - BALL_RADIUS) - (squareX + squareWidth);

                    float smallestOverlap = fmin(fabs(overlapTop), fmin(fabs(overlapBottom), fmin(fabs(overlapLeft), fabs(overlapRight))));

                    if (smallestOverlap == fabs(overlapTop))
                    {
                        trajectoryPosition.y = squareY - BALL_RADIUS;
                        velocity.y *= -DAMPING;
                    }
                    else if (smallestOverlap == fabs(overlapBottom))
                    {
                        trajectoryPosition.y = squareY + squareHeight + BALL_RADIUS;
                        velocity.y *= -DAMPING;
                    }
                    else if (smallestOverlap == fabs(overlapLeft))
                    {
                        trajectoryPosition.x = squareX - BALL_RADIUS;
                        velocity.x *= -DAMPING;
                    }
                    else if (smallestOverlap == fabs(overlapRight))
                    {
                        trajectoryPosition.x = squareX + squareWidth + BALL_RADIUS;
                        velocity.x *= -DAMPING;
                    }
                }
            }

            // Apply rolling friction when on the ground
            if (trajectoryPosition.y >= groundLevel && velocity.y == 0)
            {
                velocity.x *= FRICTION;
                if (fabs(velocity.x) < STOP_THRESHOLD.x)
                {
                    velocity.x = 0; // Stop rolling motion
                }
            }

            // Draw the trajectory line
            DrawLineV(previousPosition, trajectoryPosition, RED); // Line color in red
            previousPosition = trajectoryPosition;

            // Stop if both velocity components fall below the threshold
            if (fabs(velocity.x) < STOP_THRESHOLD.x && fabs(velocity.y) < STOP_THRESHOLD.y)
            {
                break;
            }
        }
    }
}



void Ball::drawHelpMenu(const Camera2D& camera) {
    // Fixed font size, independent of zoom level
    int fontSize = 20;   // Fixed font size, not affected by zoom
    float offsetY = 20.0f;

    // Draw the help menu text in screen-space coordinates, fixed at the top-left of the screen
    // The screen coordinates are not affected by the camera's position or zoom

    // Draw "Help Menu:" title at fixed position in screen space
    DrawText("Help Menu:", 10, offsetY, fontSize, BLACK);

    offsetY += 30.0f;
    DrawText("Press H to toggle help menu", 10, offsetY, fontSize, BLACK);

    offsetY += 30.0f;
    DrawText("Press R to reset the game", 10, offsetY, fontSize, BLACK);
    
    offsetY += 30.0f;
    DrawText("Press M to return to the menu", 10, offsetY, fontSize, BLACK);

    offsetY += 30.0f;
    DrawText("Press T to toggle trajectory", 10, offsetY, fontSize, BLACK);

    offsetY += 30.0f;
    DrawText("Press L to toggle rainbow ball", 10, offsetY, fontSize, BLACK);

    offsetY += 30.0f;
    DrawText("Press F to lock camera on ball", 10, offsetY, fontSize, BLACK);

    offsetY += 30.0f;
    DrawText("Press C to lock camera on center", 10, offsetY, fontSize, BLACK);

    offsetY += 30.0f;
    DrawText("Press WASD to pan the camera", 10, offsetY, fontSize, BLACK);

    offsetY += 30.0f;
    DrawText("Scroll to zoom the camera", 10, offsetY, fontSize, BLACK);
}



void Ball::drawDebug()
{
    //Draw minimum height terrain lines
    int font = 30;
    DrawRectangleLines(sst::cx(0), sst::cyf(sst::baseY - GRASS_HEIGHT), sst::cx(sst::baseX), sst::cyf(GRASS_HEIGHT), BLACK);
    DrawText(TextFormat("Shots: %d", shotCount), sst::cx(0), sst::cy(sst::baseY - 100), sst::cx(font), BLACK);
    DrawText(TextFormat("isRolling: %i", static_cast<int>(isRolling)), sst::cx(0), sst::cy(sst::baseY - 50), sst::cx(font), BLACK);
}

void Ball::updatePhysics(const std::vector<TerrainSquare>& terrain)
{
    isStopped = (velocity.x == 0 && velocity.y == 0);

    if (!isDragging)
    {
        ballPosition.x += velocity.x;
        ballPosition.y += velocity.y;

        //Apply Gravity
        velocity.y += GRAVITY;

        //Check for collisions
        checkCollisions(terrain);

        //Apply friction if rolling
        applyFriction();
    }
}

void Ball::checkCollisions(const std::vector<TerrainSquare>& terrain)
{
    // Minimum height collision (ground level)
    if (ballPosition.y > sst::baseY - GRASS_HEIGHT - BALL_RADIUS) 
    {
        ballPosition.y = sst::baseY - GRASS_HEIGHT - BALL_RADIUS;

        if (fabs(velocity.y) < STOP_THRESHOLD.y) 
        {
            velocity.y = 0;
            isRolling = true;
        } 
        else 
        {
            isRolling = false;
            velocity.y *= -DAMPING;
        }
    }

    // Side wall collisions
    if (ballPosition.x < BALL_RADIUS) 
    {
        ballPosition.x = BALL_RADIUS;
        velocity.x *= -DAMPING;
    } 
    else if (ballPosition.x > sst::baseX - BALL_RADIUS) 
    {
        ballPosition.x = sst::baseX - BALL_RADIUS;
        velocity.x *= -DAMPING;
    }

    // Top wall collision
    if (ballPosition.y < BALL_RADIUS) 
    {
        ballPosition.y = BALL_RADIUS;
        velocity.y *= -DAMPING;
    }

    // Terrain collisions
    for (const TerrainSquare& square : terrain) 
    {
        int squareX = square.getPosX();
        int squareWidth = square.getWidth();
        int squareY = sst::baseY - GRASS_HEIGHT - square.getHeight();
        int squareHeight = square.getHeight();

        // Check if the ball is intersecting the terrain square
        bool withinXBounds = (ballPosition.x + BALL_RADIUS > squareX) && (ballPosition.x - BALL_RADIUS < squareX + squareWidth);
        bool withinYBounds = (ballPosition.y + BALL_RADIUS > squareY) && (ballPosition.y - BALL_RADIUS < squareY + squareHeight);

        if (withinXBounds && withinYBounds) 
        {
            // Check which side the ball is colliding with and adjust accordingly
            float overlapTop = (squareY - (ballPosition.y + BALL_RADIUS));
            float overlapBottom = ((ballPosition.y - BALL_RADIUS) - (squareY + squareHeight));
            float overlapLeft = (squareX - (ballPosition.x + BALL_RADIUS));
            float overlapRight = ((ballPosition.x - BALL_RADIUS) - (squareX + squareWidth));

            // Determine the smallest overlap to resolve collision
            float smallestOverlap = fmin(fabs(overlapTop), fmin(fabs(overlapBottom), fmin(fabs(overlapLeft), fabs(overlapRight))));


            if (smallestOverlap == fabs(overlapTop)) 
            {
                // Collision with the top of the square
                ballPosition.y = squareY - BALL_RADIUS;
                if (fabs(velocity.y) < STOP_THRESHOLD.y) 
                {
                    velocity.y = 0;
                    isRolling = true;
                } 
                else 
                {
                    velocity.y *= -DAMPING;
                    isRolling = false;
                }
            } 
            else if (smallestOverlap == fabs(overlapBottom)) 
            {
                // Collision with the bottom of the square
                ballPosition.y = squareY + squareHeight + BALL_RADIUS;
                velocity.y *= -DAMPING;
            } 
            else if (smallestOverlap == fabs(overlapLeft)) 
            {
                // Collision with the left side of the square
                ballPosition.x = squareX - BALL_RADIUS;
                velocity.x *= -DAMPING;
            } 
            else if (smallestOverlap == fabs(overlapRight)) 
            {
                // Collision with the right side of the square
                ballPosition.x = squareX + squareWidth + BALL_RADIUS;
                velocity.x *= -DAMPING;
            }
        }
    }
}


void Ball::applyFriction()
{
    if (isRolling) 
    {
        velocity.x *= FRICTION;
        if (fabs(velocity.x) < STOP_THRESHOLD.x) //Once the ball has speed less than 0.1, force it to stop
        {
            velocity.x = 0;
            isRolling = false;
        }
    }    
}

void Ball::updateLogic()
{
    shotCount++;
    previousStartPosition = ballPosition;
}

Vector2 Ball::getBallPosition()
{
    return {ballPosition.x, ballPosition.y};
}

Vector2 Ball::getVelocity() {
    return velocity;
}

void Ball::updateVelocity(Vector2 newVel)
{
    velocity = newVel;
}

void Ball::updateColor(Color c)
{
    ballColor = c;
}

void Ball::updateColorOverTime(float deltaTime) {
    static float hue = 0.0f;  // Start with hue at 0
    const float hueChangeSpeed = 50.0f;  // Speed of hue change (degrees per second)

    // Increment hue based on time
    hue += hueChangeSpeed * deltaTime;
    if (hue >= 360.0f) {
        hue -= 360.0f;  // Keep hue within the range [0, 360]
    }

    // Normalize hue to the range [0, 1] for smooth transitions
    float normalizedHue = hue / 360.0f;

    // Create smooth transitions using a sine wave function to avoid sudden jumps
    float r = 0.5f * (sin(normalizedHue * 2.0f * PI) + 1.0f);  // Smooth sine wave for red
    float g = 0.5f * (sin((normalizedHue + 1.0f / 3.0f) * 2.0f * PI) + 1.0f);  // Green offset for smoother transition
    float b = 0.5f * (sin((normalizedHue + 2.0f / 3.0f) * 2.0f * PI) + 1.0f);  // Blue offset for smoother transition

    // Convert to [0, 255] range and update the ball color
    updateColor((Color){static_cast<unsigned char>(r * 255),
                        static_cast<unsigned char>(g * 255),
                        static_cast<unsigned char>(b * 255),
                        255});
}