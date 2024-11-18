#ifndef SINGLEPLAYERMATCH_H
#define SINGLEPLAYERMATCH_H

#include "raylib.h"
#include "GuiEnum.h"
#include "textureClass.h"
#include "button.h"
#include "mouse.h"
#include "ball.h"
#include "match.h"
#include "terrain.h"

/*
TODO:
    - Implement wind (other than 3)
    - Implement difficulty
    - Implement endless mode
    - Implement terrain
    - When game ends, do something other than returning to main menu
*/

class SingleplayerMatch : public Match
{
    private:
        std::vector<TerrainSquare> terrain;
    public:
        SingleplayerMatch(int difficulty, int wind, bool endlessMode);
        bool cameraShouldFollowBall;
        bool cameraShouldCenter;
        float smoothingFactor;
        void draw() override;
        void drawDebug() override;
        void updateCamera();
        GuiEvent updateLogic() override;
        Camera2D camera;
};

#endif