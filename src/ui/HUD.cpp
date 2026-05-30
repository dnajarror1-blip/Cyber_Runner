//
// Created by darwin on 18/04/26.
//
#include "ui/HUD.h"
#include <cmath>

// SECCION: HUD
// Colores propios del HUD para mantener consistencia con el estilo neon.
const Color NEO_CYAN_HUD = {0, 255, 255, 255};
const Color NEO_YELLOW_HUD = {253, 249, 0, 255};

void HUD::loadAssets()
{
    // Carga iconos pequenos que se muestran en la parte superior del gameplay.
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
    // Evita que las barras circulares se dibujen fuera del rango 0..1.
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
    Texture2D iconTexture,
    bool haloStyle
)
{
    // SECCION: Poderes HUD
    // Dibuja icono, brillo y aro de duracion para nitro o escudo.
    float clampedProgress = clampProgress(progress);
    float time = static_cast<float>(GetTime());
    float pulse = (std::sin(time * 5.0f) + 1.0f) * 0.5f;
    float orbit = time * 120.0f;
    float innerRadius = haloStyle ? 9.5f + pulse * 1.5f : 10.5f;
    float outerRadius = haloStyle ? 15.5f + pulse * 2.0f : 15.0f + pulse * 1.5f;
    Color glowColor = color;
    glowColor.a = haloStyle ? 120 : 150;

    DrawCircleGradient(
        static_cast<int>(center.x),
        static_cast<int>(center.y),
        outerRadius,
        glowColor,
        {0, 0, 0, 210}
    );

    DrawRing(
        center,
        innerRadius,
        innerRadius + 2.0f,
        orbit,
        orbit + (haloStyle ? 265.0f : 160.0f),
        64,
        color
    );

    DrawRing(
        center,
        outerRadius,
        outerRadius + 1.5f,
        -orbit * 0.7f,
        -orbit * 0.7f + (haloStyle ? 110.0f : 220.0f),
        64,
        {255, 255, 255, 165}
    );

    // Si la textura esta cargada, se dibuja centrada dentro del aro.
    if (iconTexture.id)
    {
        Rectangle source = {
            0.0f,
            0.0f,
            static_cast<float>(iconTexture.width),
            static_cast<float>(iconTexture.height)
        };

        Rectangle dest = {
            center.x - 8.0f,
            center.y - 8.0f,
            16.0f,
            16.0f
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

    // Aro externo de progreso: indica cuanto tiempo queda del poder.
    DrawRing(
        center,
        18.5f,
        21.0f,
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
    // SECCION: HUD gameplay
    // Rectangulos superiores: puntaje izquierda, velocidad centro, record derecha.
    DrawRectangle(12, 11, 150, 24, {0, 0, 0, 145});
    DrawRectangle(350, 11, 100, 24, {0, 0, 0, 145});
    DrawRectangle(640, 11, 145, 24, {0, 0, 0, 145});

    DrawText(TextFormat("PUNTAJE %i", score), 18, 16, 15, WHITE);
    DrawText(TextFormat("VEL %.0f", speed), 360, 16, 15, NEO_CYAN_HUD);
    DrawText(TextFormat("RECORD %i", highScore), 650, 16, 15, GREEN);

    // Icono del nitro solo aparece mientras el poder esta activo.
    if (nitroActive)
    {
        drawPowerIcon(
            {315.0f, 24.0f},
            SKYBLUE,
            nitroProgress,
            nitroIconTexture,
            false
        );
    }

    // Icono del escudo solo aparece mientras el poder esta activo.
    if (hasShield)
    {
        drawPowerIcon(
            {485.0f, 24.0f},
            ORANGE,
            shieldProgress,
            shieldIconTexture,
            true
        );
    }
}

void HUD::drawMenuHUD(int creditos) {
    // SECCION: HUD menu
    // Muestra el saldo de tokens en el menu principal.
    DrawText(TextFormat("TOKENS: %i", creditos), 445, 115, 15, NEO_YELLOW_HUD);
}
