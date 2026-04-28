//
// Created by darwin on 18/04/26.
//
#ifndef CYBER_RUNNER_OBSTACLE_H
#define CYBER_RUNNER_OBSTACLE_H

#include "raylib.h"

class Obstacle {
private:
    Rectangle rect;
    float speed;

public:
    // Constructor: Define posición, tamaño y velocidad inicial
    Obstacle(float x, float y, float width, float height, float speed);

    // Mueve el obstáculo y lo recicla cuando sale de la pantalla
    void update();

    // Dibuja el obstáculo en pantalla
    void draw();

    // Permite cambiar la velocidad desde afuera (para la dificultad progresiva)
    void setSpeed(float newSpeed);

    // Devuelve el rectángulo para que el Integrante B pueda detectar colisiones
    Rectangle getRect();
};

#endif //CYBER_RUNNER_OBSTACLE_H
