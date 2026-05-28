//
// Created by darwin on 18/04/26.
//
#include "ui/HUD.h"

const Color NEO_CYAN_HUD = {0, 255, 255, 255};
const Color NEO_YELLOW_HUD = {253, 249, 0, 255};

void HUD::loadAssets()
{
    nitroIconTexture = LoadTexture("assets/nitro.png");
    shieldIconTexture = LoadTexture("assets/escudo.png");
}

void HUD::unloadAssets()
{
    if (nitroIconTexture.id)
    {
        UnloadTexture(nitroIconTexture);
        nitroIconTexture = {};
    }

    if (shieldIconTexture.id)
    {
        UnloadTexture(shieldIconTexture);
        shieldIconTexture = {};
    }
}

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
    float progress,
    Texture2D iconTexture
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

    if (iconTexture.id)
    {
        Rectangle source = {
            0.0f,
            0.0f,
            static_cast<float>(iconTexture.width),
            static_cast<float>(iconTexture.height)
        };

        Rectangle dest = {
            center.x - 9.0f,
            center.y - 9.0f,
            18.0f,
            18.0f
        };

        DrawTexturePro(
            iconTexture,
            source,
            dest,
            {0.0f, 0.0f},
            0.0f,
            WHITE
        );
    }

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
            nitroProgress,
            nitroIconTexture
        );
    }

    if (hasShield)
    {
        drawPowerIcon(
            {485.0f, 22.0f},
            ORANGE,
            shieldProgress,
            shieldIconTexture
        );
    }
}

void HUD::drawMenuHUD(int creditos) {
    DrawText(TextFormat("CREDITOS: %i", creditos), 445, 115, 15, NEO_YELLOW_HUD);
}
