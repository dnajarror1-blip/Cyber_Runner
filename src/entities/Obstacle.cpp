//
// Created by darwin on 18/04/26.
//

#include "entities/Obstacle.h"

Obstacle::Obstacle(float x, float y, float width, float height, float speed) {
    rect = {x, y, width, height};
    this->speed = speed;

    droneTexture = LoadTexture("assets/dron.png");

    if (rect.y < 300) {
        type = ObstacleType::AIR;
    } else {
        type = ObstacleType::GROUND;
    }
}

Obstacle::~Obstacle()
{
    if (droneTexture.id)
    {
        UnloadTexture(droneTexture);
    }
}

Obstacle::Obstacle(Obstacle&& other) noexcept
{
    rect = other.rect;
    speed = other.speed;
    type = other.type;
    droneTexture = other.droneTexture;

    other.droneTexture = {};
}

Obstacle& Obstacle::operator=(Obstacle&& other) noexcept
{
    if (this != &other)
    {
        if (droneTexture.id)
        {
            UnloadTexture(droneTexture);
        }

        rect = other.rect;
        speed = other.speed;
        type = other.type;
        droneTexture = other.droneTexture;

        other.droneTexture = {};
    }

    return *this;
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
            rect.y = 205;        // Altura de vuelo (Dron)
            rect.width = 40;     // Ancho según el diseño Lo-Fi
            rect.height = 25;    // Alto según el diseño Lo-Fi

            type = ObstacleType::AIR;
        } else {
            rect.y = 310;        // Altura de suelo (Barrera)
            rect.width = 25;
            rect.height = 40;

            type = ObstacleType::GROUND;
        }
    }
}

// Implementación de la nueva función para subir dificultad
void Obstacle::setSpeed(float newSpeed) {
    this->speed = newSpeed;
}

void Obstacle::draw() {
    // Dibujamos de distinto color para saber qué tipo de obstáculo es
    if (type == ObstacleType::AIR) {

        if (droneTexture.id == 0)
        {
            DrawRectangleRec(rect, YELLOW);
            return;
        }

        Rectangle source = {
            0,
            0,
            (float)droneTexture.width,
            (float)droneTexture.height
        };

        Rectangle dest = {
            rect.x,
            rect.y,
            rect.width,
            rect.height
        };

        DrawTexturePro(
            droneTexture,
            source,
            dest,
            {0,0},
            0.0f,
            WHITE
        );

    } else {
        DrawRectangleRec(rect, MAGENTA);
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
        rect.y = 205;
        rect.width = 40;
        rect.height = 25;

        type = ObstacleType::AIR;
    }
    else
    {
        rect.y = 310;
        rect.width = 25;
        rect.height = 40;

        type = ObstacleType::GROUND;
    }
}