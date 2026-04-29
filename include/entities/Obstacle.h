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
    // El constructor que ya tenías
    Obstacle(float x, float y, float width, float height, float speed);

    // Actualiza la posición y cambia el tipo de obstáculo al reaparecer
    void update();

    // Dibuja el obstáculo (mientras no tengas los sprites finales)
    void draw();

    // FUNCIÓN NUEVA: Permite que la velocidad suba con el tiempo
    void setSpeed(float newSpeed);

    // Devuelve el rectángulo para detectar choques
    Rectangle getRect();
};

#endif
