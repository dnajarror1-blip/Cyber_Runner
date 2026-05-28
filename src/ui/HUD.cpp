//
// Created by darwin on 18/04/26.
//
#include "ui/HUD.h"

const Color NEO_CYAN_HUD = {0, 255, 255, 255};
const Color NEO_YELLOW_HUD = {253, 249, 0, 255};

static float clampProgress(float value)
{
    if (value < 0.0f)
    {
        return 0.0f;
    }

    if (value > 1.0f)
    {
        return 1.0f;
    }

    return value;
}

static void drawPowerIcon(
    Vector2 center,
    Color color,
    float progress
)
{
    float clampedProgress = clampProgress(progress);

    DrawCircleGradient(
        static_cast<int>(center.x),
        static_cast<int>(center.y),
        13.0f,
        color,
        {0, 0, 0, 210}
    );

    DrawCircleLines(
        static_cast<int>(center.x),
        static_cast<int>(center.y),
        14.0f,
        WHITE
    );

    DrawRing(
        center,
        16.0f,
        19.0f,
        -90.0f,
        -90.0f + 360.0f * clampedProgress,
        48,
        color
    );
}

void HUD::drawGameHUD(
    float speed,
    int score,
    int highScore,
    bool nitroActive,
    bool hasShield,
    float nitroProgress,
    float shieldProgress
) {
    DrawRectangle(12, 11, 150, 24, {0, 0, 0, 145});
    DrawRectangle(350, 11, 100, 24, {0, 0, 0, 145});
    DrawRectangle(640, 11, 145, 24, {0, 0, 0, 145});

    DrawText(TextFormat("PUNTAJE %i", score), 18, 16, 15, WHITE);
    DrawText(TextFormat("VEL %.0f", speed), 360, 16, 15, NEO_CYAN_HUD);
    DrawText(TextFormat("RECORD %i", highScore), 650, 16, 15, GREEN);

    if (nitroActive)
    {
        drawPowerIcon(
            {315.0f, 22.0f},
            SKYBLUE,
            nitroProgress
        );
    }

    if (hasShield)
    {
        drawPowerIcon(
            {485.0f, 22.0f},
            ORANGE,
            shieldProgress
        );
    }
}

void HUD::drawMenuHUD(int creditos) {
    DrawText(TextFormat("CREDITOS: %i", creditos), 445, 115, 15, NEO_YELLOW_HUD);
}
