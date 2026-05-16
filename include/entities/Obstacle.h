#pragma once

#include "raylib.h"

enum class ObstacleType {
    GROUND,
    AIR
};

class Obstacle {
private:
    Rectangle rect;
    float speed;

    ObstacleType type;

    Texture2D droneTexture {};

public:
    // El constructor que ya tenías
    Obstacle(float x, float y, float width, float height, float speed);

    ~Obstacle();

    Obstacle(const Obstacle&) = delete;
    Obstacle& operator=(const Obstacle&) = delete;

    Obstacle(Obstacle&& other) noexcept;
    Obstacle& operator=(Obstacle&& other) noexcept;

    // Actualiza la posición y cambia el tipo de obstáculo al reaparecer
    void update(float deltaTime);

    // Dibuja el obstáculo (mientras no tengas los sprites finales)
    void draw();

    // FUNCIÓN NUEVA: Permite que la velocidad suba con el tiempo
    void setSpeed(float newSpeed);

    void forceRespawn();

    // Devuelve el rectángulo para detectar choques
    Rectangle getRect();
};