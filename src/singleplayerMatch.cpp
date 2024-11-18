#include "singleplayerMatch.h"

SingleplayerMatch::SingleplayerMatch(int difficulty, int wind, bool endlessMode)
{
    //Settings for the match, so far serve no purpose
    this->difficulty = difficulty;
    this->wind = wind;
    this->endlessMode = endlessMode;
    end = false;

    //Load mouse
    mouse = Mouse();
    //Load golfball
    golfball = Ball(WHITE);

    //Create golfballButton
    Vector2 ballPosVec = golfball.getBallPosition();
    addButton("Ball", {ballPosVec.x - 13, ballPosVec.y - 13, 28, 28});
    //Create Hole at ground level at opposite playing side
    addButton("Hole", {sst::baseX - 100, sst::baseY - GRASS_HEIGHT - 10, 20, 20});

    //Terrain
    Terrain terrainGenerator;
    terrain = terrainGenerator.DrawTerrain(difficulty);

    // Initialize camera
    camera.target = {sst::baseX / 2.0f, sst::baseY / 2.0f}; // Set camera target to center of screen
    camera.offset = {sst::cx(sst::baseX) / 2.0f, sst::cyf(sst::baseY) / 2.0f}; // Center of the screen
    camera.rotation = 0.0f;
    camera.zoom = 1.0f; // Default zoom level
    cameraShouldFollowBall = false; // Start with camera not following the ball
    cameraShouldCenter = false;
    smoothingFactor = 0.1f; // Adjust smoothing (lower is slower)

}

void SingleplayerMatch::draw() {
    // Start using camera
    BeginMode2D(camera);

    // Draw the environment and objects relative to the camera
    DrawRectangle(sst::cx(0), sst::cyf(sst::baseY - GRASS_HEIGHT), sst::cx(sst::baseX), sst::cyf(GRASS_HEIGHT), GREEN);
    DrawRectangle(sst::cx(0), sst::cy(0), sst::cx(sst::baseX), sst::cyf(sst::baseY - GRASS_HEIGHT), BLUE);
    DrawRectangle(sst::cx(sst::baseX - 100), sst::cy(sst::baseY - GRASS_HEIGHT - 10), sst::cx(20), sst::cx(20), MAGENTA);

    // Draw terrain
    for (const TerrainSquare& square : terrain) {
        int yPos = sst::baseY - GRASS_HEIGHT - square.getHeight();
        int posX = square.getPosX();
        int width = square.getWidth();
        DrawRectangle(posX, yPos, width, square.getHeight(), GREEN); // Call the draw method for each square
    }

    // Draw the ball (relative to the camera)
    golfball.draw(camera, terrain);

    // End using camera (before drawing UI text)
    EndMode2D();
}

void SingleplayerMatch::drawDebug()
{
    BeginMode2D(camera);

    golfball.drawDebug();
    DrawRectangleRec(mouse.mouseHitbox(), PURPLE);
    for (int i = 0; i < amountOfButtons(); i++)
    {
        DrawRectangleLinesEx(buttons[i].getBounds(), 5, PURPLE);
    }

    EndMode2D();

    // UI elements or debug information that you want to stay fixed on the screen
    int font = 20;
    DrawText(TextFormat("Buttons[%i]", buttonClicked()), 0, 0, sst::cx(font), BLACK);
    DrawText(TextFormat("Wind: %i", wind), 0, sst::cy(40), sst::cx(font), BLACK);
}


void SingleplayerMatch::updateCamera() {
    // Variables to store the desired target position
    Vector2 desiredTarget = camera.target;

    // Handle panning controls (camera movement when keys are held down)
    if (IsKeyDown(KEY_A)) {
        desiredTarget.x -= 250;  // Move camera left (increase speed)
        cameraShouldFollowBall = false;  // Stop following the ball while panning
        cameraShouldCenter = false; // Stop center
    }
    if (IsKeyDown(KEY_D)) {
        desiredTarget.x += 250;  // Move camera right (increase speed)
        cameraShouldFollowBall = false;  // Stop following the ball while panning
        cameraShouldCenter = false; // Stop center
    }
    if (IsKeyDown(KEY_W)) {
        desiredTarget.y -= 250;  // Move camera up (increase speed)
        cameraShouldFollowBall = false;  // Stop following the ball while panning
        cameraShouldCenter = false; // Stop center
    }
    if (IsKeyDown(KEY_S)) {
        desiredTarget.y += 250;  // Move camera down (increase speed)
        cameraShouldFollowBall = false;  // Stop following the ball while panning
        cameraShouldCenter = false; // Stop center
    }

    // Smoothly transition the camera towards the desired target
    camera.target.x += (desiredTarget.x - camera.target.x) * smoothingFactor;
    camera.target.y += (desiredTarget.y - camera.target.y) * smoothingFactor;

    // Reset to follow the ball when `KEY_F` is pressed
    if (IsKeyPressed(KEY_F)) {
        cameraShouldFollowBall = !cameraShouldFollowBall;
    }

    // Reset to follow centre when 'KEY_C' is pressed
    if (IsKeyPressed(KEY_C)) {
        cameraShouldCenter = !cameraShouldCenter;
    }

    // Camera following ball logic (when `cameraShouldFollowBall` is true)
    if (cameraShouldFollowBall) {
        // Get the ball's position (target for the camera)
        Vector2 targetPosition = golfball.getBallPosition();
        // Smoothly move the camera towards the ball's position
        camera.target.x += (targetPosition.x - camera.target.x) * smoothingFactor;
        camera.target.y += (targetPosition.y - camera.target.y) * smoothingFactor;
    }

    // Camera following ball logic (when `cameraShouldFollowBall` is true)
    if (cameraShouldCenter) {
        Vector2 targetPosition = {sst::baseX / 2.0f, sst::baseY / 2.0f};
        float targetZoom = 1.0f;

        // Smoothly move the camera towards the center position
        camera.target.x += (targetPosition.x - camera.target.x) * smoothingFactor;
        camera.target.y += (targetPosition.y - camera.target.y) * smoothingFactor;
        camera.zoom += (targetZoom - camera.zoom) * smoothingFactor;
    }

    // Zoom camera (mouse wheel)
    camera.zoom += GetMouseWheelMove() * 0.25f;

    // Limit zoom range
    if (camera.zoom < 0.5f) camera.zoom = 0.5f;
    if (camera.zoom > 20.0f) camera.zoom = 20.0f;
}


GuiEvent SingleplayerMatch::updateLogic()
{
    // Update mouse position
    
    mouse.updateMousePosition();
    golfball.updatePhysics(terrain);

    // Update ball button position
    // Convert ball position to screen-space for button bounds
    Vector2 screenBallPos = GetWorldToScreen2D(golfball.getBallPosition(), camera);
    float buttonSize = 28 * camera.zoom; // Adjust size for zoom level
    buttons[0].updateButtonBounds({screenBallPos.x - buttonSize / 2, screenBallPos.y - buttonSize / 2, buttonSize, buttonSize});

    // Update camera logic
    updateCamera();

    // Update ball color
    if (IsKeyPressed(KEY_L)) {
        golfball.toggleRainbow = !golfball.toggleRainbow;
    }

    // Return to menu with "KEY_M"
    if (IsKeyPressed(KEY_M)) {
        end = true;
        return OpenStartingMenu;
    }

    // Toggle trajectory
    if (IsKeyPressed(KEY_T)) {
        golfball.showTrajectory = !golfball.showTrajectory;
    }

    // Check for 'H' key press to toggle the help menu
    if (IsKeyPressed(KEY_H))
    {
        golfball.showHelpMenu = !golfball.showHelpMenu;
    }

    // Reset ball
    if (IsKeyPressed(KEY_R)) {
        golfball = Ball(WHITE);
        cameraShouldCenter = true;
        cameraShouldFollowBall = false;
    }

    // Check if the ball has stopped on the hole
    if (CheckCollisionRecs(buttons[0].getBounds(), buttons[1].getBounds())) // && golfball.isStopped)
    {
        end = true;
        return OpenStartingMenu; // Victory condition
    }

    // Ball drag-and-launch logic
    if (golfball.isStopped)
    {
        Vector2 worldMousePos = GetScreenToWorld2D(mouse.position(), camera); // World-space mouse position

        // Start dragging
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && !golfball.isDragging)
        {
            // Convert ball world position to screen position
            Vector2 screenBallPosition = GetWorldToScreen2D(golfball.getBallPosition(), camera);

            // Check if mouse position is inside the ball's screen-space bounds
            float ballScreenRadius = BALL_RADIUS * camera.zoom; // Scale the radius by zoom level
            if (CheckCollisionPointCircle(mouse.position(), screenBallPosition, ballScreenRadius))
            {
                // Begin dragging
                golfball.isDragging = true;
                golfball.startDrag = golfball.getBallPosition(); // Start drag from the ball's world position
                golfball.currentDrag = GetScreenToWorld2D(mouse.position(), camera); // Track mouse in world space

                golfball.updateVelocity({0, 0});
            }
        }


        // Update drag
        else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && golfball.isDragging)
        {
            golfball.currentDrag = worldMousePos; // Update drag to current mouse position
        }
        // Release drag
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && golfball.isDragging)
        {
            Vector2 dragVector = {golfball.startDrag.x - golfball.currentDrag.x,
                                  golfball.startDrag.y - golfball.currentDrag.y};
            golfball.updateVelocity({dragVector.x * LAUNCH_SCALE, dragVector.y * LAUNCH_SCALE});
            golfball.isDragging = false;

            golfball.isRolling = (wind == 3); // Set rolling state based on wind
            golfball.updateLogic();
        }
    }

    return Nothing;
}