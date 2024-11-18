#include "program.h"
// g++ ./src/*.cpp -o golf-game.exe -I./include -L./lib -lraylibdll -lkernel32 -luser32 -lgdi32 -lm -lwinpthread -mwindows

/*
Updates:
Camera follows ball (F)
Camera follows center (C)
Camera zoom (Mousewheel)
Camera pan (WASD)
Exit game (Close Button)
Return to menu (M)
Trajectory path toggle (T)
Help menu (H)
Rainbow ball (P)
*/

int main()
{
    Program program;
    program.loop();
    program.close();
    return 0;
}