#pragma once
#include "raylib.h"

class HUD {
public:
    void drawGameHUD(
        float speed,
        int score,
        int highScore,
        bool nitroActive,
        bool hasShield,
        float nitroProgress,
        float shieldProgress
    );
    void drawMenuHUD(int creditos);
};
