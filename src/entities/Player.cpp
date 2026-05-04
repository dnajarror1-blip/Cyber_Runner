//
// Created by darwin on 18/04/26.
//
#include "entities/Player.h"

void Player::update(float deltaTime) {

    // saltar o hace doble salto
    if (IsKeyPressed(KEY_SPACE) && saltosDisponibles > 0)
    {
        velocidadY = fuerzaSalto;
        saltosDisponibles--;
        enSuelo = false;
    }
    // Aplicar gravedad
    velocidadY += gravedad * deltaTime;

    // Limitar velocidad máxima de caída
    if (velocidadY > velocidadCaidaMaxima)
    {
        velocidadY = velocidadCaidaMaxima;
    }

    // Mover jugador verticalmente
    rect.y += velocidadY * deltaTime;

    // Detectar suelo
    if (rect.y >= sueloY)
    {
        rect.y = sueloY;
        velocidadY = 0.0f;
        ensuelo = true;
        saltosDisponibles = 2;
    }
}

void Player::draw() {
    DrawRectangleRec(rect, { 0, 255, 255, 255 }); // cyan


}

Rectangle Player::getRect() {
    return rect;
}