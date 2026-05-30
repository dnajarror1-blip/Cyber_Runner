#include "entities/Obstacle.h"
#include <cmath>

Obstacle::Obstacle(float x, float y, float width, float height, float speed) {
    // SECCION: Obstaculos - posicion y tamano
    // x, y, width y height definen el hitbox inicial que tambien se usa para dibujar.
    rect = {x, y, width, height};
    this->speed = speed;
    baseY = y;
    verticalTimer = 0.0f;
    verticalPhase = static_cast<float>(GetRandomValue(0, 628)) / 100.0f;

    droneTexture = LoadTexture("assets/dron.png");
    groundTexture = LoadTexture("assets/groundobstacle.png");

    // La altura inicial decide si el obstaculo empieza como dron o barrera de suelo.
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
        groundTexture = other.groundTexture;

        other.droneTexture = {};
        other.groundTexture = {};
    }

    return *this;
}

void Obstacle::update(float deltaTime) {
    // SECCION: Movimiento de obstaculos
    // 1. El movimiento constante a la izquierda
    float speedMultiplier = type == ObstacleType::AIR ? 1.18f : 1.0f;

    rect.x -= speed * speedMultiplier * deltaTime;

    // Los drones tienen una oscilacion vertical para que no sean estaticos.
    if (type == ObstacleType::AIR)
    {
        verticalTimer += deltaTime;
        rect.y = baseY + static_cast<float>(
            std::sin(verticalTimer * 4.0f + verticalPhase)
        ) * 30.0f;
    }

    // 2. Si sale de la pantalla, lo "teletransportamos" y cambiamos su forma
    // Al salir de pantalla, el obstaculo reaparece delante del jugador.
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
    // SECCION: Dibujo de obstaculos
    // Se dibuja segun el tipo actual: dron en aire o barrera en suelo.
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

        // dest usa el rectangulo de colision como destino visual.
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

        // dest usa el rectangulo de colision como destino visual.
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
    // Usado cuando el jugador tenia escudo: evita que el mismo obstaculo choque otra vez.
    respawn(800 + GetRandomValue(600, 950));
}

void Obstacle::respawn(float x)
{
    // SECCION: Respawn de obstaculos
    // x define donde reaparece; y, width y height se ajustan segun tipo.
    rect.x = x;
    verticalTimer = 0.0f;
    verticalPhase = static_cast<float>(GetRandomValue(0, 628)) / 100.0f;

    // 0 crea dron; 1 crea barrera de suelo.
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
