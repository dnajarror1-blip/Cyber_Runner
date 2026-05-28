//
// Created by darwin on 18/04/26.
//
#include "ui/HUD.h"

const Color NEO_CYAN_HUD = {0, 255, 255, 255};
const Color NEO_YELLOW_HUD = {253, 249, 0, 255};

void HUD::drawGameHUD(float speed, int creditos, int score, int highScore, bool nitroActive, bool hasShield) {
    DrawRectangle(8, 8, 230, 44, {0, 0, 0, 180});
    DrawRectangleLines(8, 8, 230, 44, NEO_CYAN_HUD);
    DrawText(TextFormat("PUNTAJE: %i", score), 20, 21, 20, WHITE);

    DrawRectangle(8, 62, 190, 38, {0, 0, 0, 180});
    DrawRectangleLines(8, 62, 190, 38, NEO_YELLOW_HUD);
    DrawText(TextFormat("CREDITOS: %i", creditos), 20, 72, 18, NEO_YELLOW_HUD);

    DrawRectangle(560, 8, 230, 44, {0, 0, 0, 180});
    DrawRectangleLines(560, 8, 230, 44, GREEN);
    DrawText(TextFormat("MEJOR: %i", highScore), 572, 21, 20, GREEN);

    DrawRectangle(600, 62, 190, 38, {0, 0, 0, 180});
    DrawRectangleLines(600, 62, 190, 38, NEO_CYAN_HUD);
    DrawText(TextFormat("VEL: %.0f", speed), 612, 72, 18, NEO_CYAN_HUD);

    if (nitroActive)
    {
        DrawRectangle(315, 56, 170, 32, {0, 0, 0, 180});
        DrawRectangleLines(315, 56, 170, 32, SKYBLUE);
        DrawText("NITRO ACTIVO", 328, 65, 15, SKYBLUE);
    }

    if (hasShield)
    {
        DrawRectangle(315, 8, 170, 32, {0, 0, 0, 180});
        DrawRectangleLines(315, 8, 170, 32, ORANGE);
        DrawText("ESCUDO ACTIVO", 328, 17, 15, ORANGE);
    }
}

void HUD::drawMenuHUD(int creditos) {
    DrawText(TextFormat("CREDITOS: %i", creditos), 445, 115, 15, NEO_YELLOW_HUD);
}
