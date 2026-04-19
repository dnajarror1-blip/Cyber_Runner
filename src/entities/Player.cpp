//
// Created by darwin on 18/04/26.
//
#include "entities/Player.h"

const float GRAVEDAD = 0.6f;

void Player::update() {

    if (IsKeyPressed(KEY_SPACE) && enSuelo) {
        velocidadY = -12.0f;
        enSuelo = false;
    }

    velocidadY += GRAVEDAD;
    rect.y += velocidadY;

    if (rect.y >= 300) {
        rect.y = 300;
        enSuelo = true;
    }
}

void Player::draw() {
    DrawRectangleRec(rect, { 0, 255, 255, 255 }); // cyan


}

Rectangle Player::getRect() {
    return rect;
}