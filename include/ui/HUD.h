#pragma once
#include "raylib.h"

class HUD {
public:
    void loadAssets();
    void unloadAssets();

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

private:
    Texture2D nitroIconTexture {};
    Texture2D shieldIconTexture {};
};
