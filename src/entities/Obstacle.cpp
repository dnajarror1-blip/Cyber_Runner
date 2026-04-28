//
// Created by darwin on 18/04/26.
//

#include <entities/Obstacle.h>

Obstacle::Obstacle(float x, float y, float width, float height, float speed) {
    rect = {x, y, width, height};
    this->speed = speed;
}

void Obstacle::update() {
    // 1. Mover el obstáculo a la izquierda
    rect.x -= speed;

    // 2. Lógica de "Reciclaje" (Cuando sale de la pantalla)
    if (rect.x + rect.width < 0) {
        // Teletransportar a la derecha con una distancia aleatoria
        rect.x = 800 + GetRandomValue(0, 300);

        // --- AQUÍ PONES LA LÓGICA DE VARIEDAD (TU TAREA) ---
        // Decidimos al azar si el siguiente obstáculo será alto o bajo
        // 0 = Suelo, 1 = Aire
        if (GetRandomValue(0, 1) == 0) {
            rect.y = 310; // Altura de obstáculos terrestres
            rect.height = 40; // Por ejemplo, una valla es más alta
        } else {
            rect.y = 220; // Altura para que el robot pase por debajo (Dron)
            rect.height = 20; // Un dron es más delgado
        }
    }
}

// --- ESTO ES NUEVO: Agrégalo debajo de update ---
// Permitirá que el Integrante A o tú mismo aumenten la velocidad desde el main
void Obstacle::setSpeed(float newSpeed) {
    this->speed = newSpeed;
}

void Obstacle::draw() {
    // Si es un obstáculo de aire, lo pintamos de otro color temporalmente
    if (rect.y < 300) {
        DrawRectangleRec(rect, {255, 255, 0, 255}); // Amarillo para drones
    } else {
        DrawRectangleRec(rect, {255, 0, 255, 255}); // Magenta para suelo
    }
}

Rectangle Obstacle::getRect() {
    return rect;
}
