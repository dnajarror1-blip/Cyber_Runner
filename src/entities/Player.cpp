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
        enSuelo = true;
        saltosDisponibles = 2;
    }
}

void Player::draw() {
    DrawRectangleRec(rect, { 0, 255, 255, 255 }); // cyan

    // Efecto visual simple cuando está en el aire
    if (!enSuelo)
    {
        DrawCircle(rect.x + rect.width / 2, rect.y + rect.height + 8, 8, ORANGE);
    }
}

Rectangle Player::getRect() {
    return rect;
}