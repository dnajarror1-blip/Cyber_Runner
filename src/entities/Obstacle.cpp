#include "entities/Obstacle.h"
#include <cmath>

Obstacle::Obstacle(float x, float y, float width, float height, float speed) {
    rect = {x, y, width, height};
    this->speed = speed;
    baseY = y;
    verticalTimer = 0.0f;
    verticalPhase = static_cast<float>(GetRandomValue(0, 628)) / 100.0f;

    droneTexture = LoadTexture("assets/dron.png");
    groundTexture = LoadTexture("assets/groundobstacle.png");

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
    if (groundTexture.id)
    {
        UnloadTexture(groundTexture);
    }
}

Obstacle::Obstacle(Obstacle&& other) noexcept
{
    rect = other.rect;
    speed = other.speed;
    baseY = other.baseY;
    verticalTimer = other.verticalTimer;
    verticalPhase = other.verticalPhase;
    type = other.type;
    droneTexture = other.droneTexture;
    groundTexture = other.groundTexture;

    other.droneTexture = {};
    other.groundTexture = {};
}

Obstacle& Obstacle::operator=(Obstacle&& other) noexcept
{
    if (this != &other)
    {
        if (droneTexture.id)
        {
            UnloadTexture(droneTexture);
        }
        if (groundTexture.id)
        {
            UnloadTexture(groundTexture);
        }

        rect = other.rect;
        speed = other.speed;
        baseY = other.baseY;
        verticalTimer = other.verticalTimer;
        verticalPhase = other.verticalPhase;
        type = other.type;
        droneTexture = other.droneTexture;

        other.droneTexture = {};
    }

    return *this;
}

void Obstacle::update(float deltaTime) {
    // 1. El movimiento constante a la izquierda
    float speedMultiplier = type == ObstacleType::AIR ? 1.18f : 1.0f;

    rect.x -= speed * speedMultiplier * deltaTime;

    if (type == ObstacleType::AIR)
    {
        verticalTimer += deltaTime;
        rect.y = baseY + static_cast<float>(
            std::sin(verticalTimer * 4.0f + verticalPhase)
        ) * 30.0f;
    }

    // 2. Si sale de la pantalla, lo "teletransportamos" y cambiamos su forma
    if (rect.x + rect.width < 0) {
        respawn(800 + GetRandomValue(500, 900));
        return;
#if 0

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
#endif
    }
}

// Implementación de la nueva función para subir dificultad
void Obstacle::setSpeed(float newSpeed) {
    this->speed = newSpeed;
}

void Obstacle::draw()
{
    if (type == ObstacleType::AIR)
    {
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
    }
    else
    {
        if (groundTexture.id == 0)
        {
            DrawRectangleRec(rect, MAGENTA);
            return;
        }

        Rectangle source = {
            0,
            0,
            (float)groundTexture.width,
            (float)groundTexture.height
        };

        Rectangle dest = {
            rect.x,
            rect.y,
            rect.width,
            rect.height
        };

        DrawTexturePro(
            groundTexture,
            source,
            dest,
            {0,0},
            0.0f,
            WHITE
        );

        // DrawRectangleLinesEx(rect, 1, RED);
    }
}

Rectangle Obstacle::getRect() {
    return rect;
}

void Obstacle::forceRespawn()
{
    respawn(800 + GetRandomValue(600, 950));
}

void Obstacle::respawn(float x)
{
    rect.x = x;
    verticalTimer = 0.0f;
    verticalPhase = static_cast<float>(GetRandomValue(0, 628)) / 100.0f;

    if (GetRandomValue(0, 1) == 0)
    {
        rect.y = 205;
        baseY = rect.y;
        rect.width = 40;
        rect.height = 25;

        type = ObstacleType::AIR;
    }
    else
    {
        rect.y = 310;
        baseY = rect.y;
        rect.width = 25;
        rect.height = 40;

        type = ObstacleType::GROUND;
    }


}
ObstacleType Obstacle::getType() const
{
    return type;
}
