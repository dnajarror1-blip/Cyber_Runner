//
// Created by darwin on 18/04/26.
//
#include "ui/HUD.h"

const Color NEO_CYAN_HUD = {0, 255, 255, 255};
const Color NEO_YELLOW_HUD = {253, 249, 0, 255};

void HUD::drawGameHUD(float speed, int creditos) {
    DrawText(TextFormat("VELOCIDAD: %.2f", speed), 10, 10, 20, NEO_CYAN_HUD);
    DrawText(TextFormat("CREDITOS: %i", creditos), 10, 35, 20, NEO_YELLOW_HUD);
}

void HUD::drawMenuHUD(int creditos) {
    DrawText(TextFormat("CREDITOS: %i", creditos), 280, 400, 15, NEO_YELLOW_HUD);
}