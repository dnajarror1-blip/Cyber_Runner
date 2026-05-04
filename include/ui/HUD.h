#pragma once
#include "raylib.h"

class HUD {
public:
    void drawGameHUD(float speed, int creditos, int score, int highScore);
    void drawMenuHUD(int creditos);
};