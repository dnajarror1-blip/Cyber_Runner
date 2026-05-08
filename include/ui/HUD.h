#pragma once
#include "raylib.h"

class HUD {
public:
    void drawGameHUD(float speed, int creditos, int score, int highScore, bool nitroActive);
    void drawMenuHUD(int creditos);
};