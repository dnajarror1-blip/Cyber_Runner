//
// Created by darwin on 18/04/26.
//

#include "entities/Obstacle.h" // Verifica que esta ruta sea la correcta en tu proyecto

Obstacle::Obstacle(float x, float y, float width, float height, float speed) {
    rect = {x, y, width, height};
    this->speed = speed;
}

void Obstacle::update(float deltaTime) {
    // 1. El movimiento constante a la izquierda
    rect.x -= speed * deltaTime;

    // 2. Si sale de la pantalla, lo "teletransportamos" y cambiamos su forma
    if (rect.x + rect.width < 0) {
        rect.x = 800 + GetRandomValue(0, 300);

        // --- TU LÓGICA DE VARIACIÓN ---
        // Decidimos al azar: 0 es Dron, 1 es Barrera
        if (GetRandomValue(0, 1) == 0) {
            rect.y = 220;        // Altura de vuelo (Dron)
            rect.width = 40;     // Ancho según el diseño Lo-Fi
            rect.height = 25;    // Alto según el diseño Lo-Fi
        } else {
            rect.y = 310;        // Altura de suelo (Barrera)
            rect.width = 25; 
            rect.height = 45; 
        }
    }
}

// Implementación de la nueva función para subir dificultad
void Obstacle::setSpeed(float newSpeed) {
    this->speed = newSpeed;
}

void Obstacle::draw() {
    // Dibujamos de distinto color para saber qué tipo de obstáculo es
    if (rect.y < 300) {
        DrawRectangleRec(rect, YELLOW); // Amarillo para el Dron
    } else {
        DrawRectangleRec(rect, MAGENTA); // Magenta para la Barrera
    }
}

Rectangle Obstacle::getRect() {
    return rect;
}

void Obstacle::forceRespawn()
{
    rect.x = 800 + GetRandomValue(300, 800);

    if (GetRandomValue(0, 1) == 0)
    {
        rect.y = 220;
        rect.width = 40;
        rect.height = 25;
    }
    else
    {
        rect.y = 310;
        rect.width = 25;
        rect.height = 45;
    }
}