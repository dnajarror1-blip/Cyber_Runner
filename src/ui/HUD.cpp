//
// Created by darwin on 18/04/26.
//
#include "ui/HUD.h"

const Color NEO_CYAN_HUD = {0, 255, 255, 255};
const Color NEO_YELLOW_HUD = {253, 249, 0, 255};

void HUD::drawGameHUD(float speed, int creditos, int score, int highScore, bool nitroActive) {
    DrawText(TextFormat("VELOCIDAD: %.0f", speed), 10, 10, 20, NEO_CYAN_HUD);
    DrawText(TextFormat("CREDITOS: %i", creditos), 10, 35, 20, NEO_YELLOW_HUD);
    DrawText(TextFormat("PUNTAJE: %i", score), 10, 60, 20, WHITE);
    DrawText(TextFormat("MEJOR: %i", highScore), 10, 85, 20, GREEN);

    if (nitroActive)
    {
        DrawText("NITRO ACTIVO", 600, 20, 20, SKYBLUE);
    }
}

void HUD::drawMenuHUD(int creditos) {
    DrawText(TextFormat("CREDITOS: %i", creditos), 280, 400, 15, NEO_YELLOW_HUD);
}