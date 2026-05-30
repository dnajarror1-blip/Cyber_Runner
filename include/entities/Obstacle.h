#pragma once

#include "raylib.h"

enum class ObstacleType {
    GROUND,
    AIR
};

// SECCION: Obstaculos
// Representa barreras de suelo y drones; Game los mueve, dibuja y revisa colisiones.
class Obstacle {
private:
    // Hitbox y posicion visual del obstaculo.
    Rectangle rect;
    // Velocidad recibida desde Game para sincronizar dificultad.
    float speed;
    // Altura base usada por drones para oscilar verticalmente.
    float baseY;
    float verticalTimer;
    float verticalPhase;

    // Define si se dibuja como obstaculo de suelo o dron.
    ObstacleType type;

    Texture2D droneTexture {};
    Texture2D groundTexture {};

public:
    // El constructor que ya tenías
    Obstacle(float x, float y, float width, float height, float speed);

    ~Obstacle();

    Obstacle(const Obstacle&) = delete;
    Obstacle& operator=(const Obstacle&) = delete;

    Obstacle(Obstacle&& other) noexcept;
    Obstacle& operator=(Obstacle&& other) noexcept;

    // Actualiza la posición y cambia el tipo de obstáculo al reaparecer
    // FUNCION: Mueve el obstaculo, anima drones y reaparece si sale de pantalla.
    void update(float deltaTime);

    // Dibuja el obstáculo (mientras no tengas los sprites finales)
    // FUNCION: Dibuja el sprite correspondiente al tipo.
    void draw();

    // FUNCIÓN NUEVA: Permite que la velocidad suba con el tiempo
    // FUNCION: Recibe la velocidad global actual del gameplay.
    void setSpeed(float newSpeed);

    // FUNCION: Fuerza reposicionamiento, usado cuando el escudo absorbe un choque.
    void forceRespawn();
    // FUNCION: Reubica y decide si aparece como suelo o aire.
    void respawn(float x);

    // Devuelve el rectángulo para detectar choques
    // FUNCION: Hitbox usado por checkCollisions().
    Rectangle getRect();

    ObstacleType getType() const;
};
