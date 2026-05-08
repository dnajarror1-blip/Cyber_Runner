//
// Created by darwin on 18/04/26.
//

#include "entities/Player.h"

Player::Player()
{
    // Cargar sprite del robot
    texture = LoadTexture("assets/player.png");

    totalFrames = 4;

    frameWidth =
        static_cast<float>(texture.width) /
        static_cast<float>(totalFrames);

    frameHeight =
        static_cast<float>(texture.height);

    frameRec = {
        0.0f,
        0.0f,
        frameWidth,
        frameHeight
    };
}

Player::~Player()
{
    // Liberar VRAM correctamente
    if (texture.id != 0)
    {
        UnloadTexture(texture);
    }
}

void Player::update(float deltaTime)
{
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

    // SISTEMA DE ANIMACION
    if (enSuelo)
    {
        frameTime += deltaTime;

        if (frameTime >= frameSpeed)
        {
            frameTime = 0.0f;

            frameActual++;

            // Frames 0-2 = correr
            if (frameActual > 2)
            {
                frameActual = 0;
            }
        }
    }
    else
    {
        // Frame salto
        frameActual = 3;
    }

    frameRec.x =
        frameWidth *
        static_cast<float>(frameActual);
}

void Player::draw()
{
    // Fallback seguro si falla el asset
    if (texture.id == 0)
    {
        DrawRectangleRec(rect, {0, 255, 255, 255});
        return;
    }

    // TAMAÑO VISUAL DESACOPLADO DE HITBOX
    float drawWidth =
        rect.width * visualScale;

    float drawHeight =
        rect.height * visualScale;

    // CENTRADO VISUAL
    float offsetX =
        (rect.width - drawWidth) / 2.0f;

    // AJUSTE CRITICO VISUAL
    float offsetY =
        (drawHeight - rect.height) - 20.0f;

    Rectangle dest = {
        rect.x + offsetX,
        rect.y - offsetY + hitboxOffsetY,
        drawWidth,
        drawHeight
    };

    DrawTexturePro(
        texture,
        frameRec,
        dest,
        {0, 0},
        0.0f,
        WHITE
    );

    // DEBUG HITBOX
    // Mantener debugging original
    DrawRectangleLinesEx(
        rect,
        1.0f,
        SKYBLUE
    );

    // Efecto visual simple cuando está en el aire
    if (!enSuelo)
    {
        DrawCircle(
            rect.x + rect.width / 2.0f,
            rect.y + rect.height + 8.0f,
            6.0f,
            ORANGE
        );
    }
}

Rectangle Player::getRect()
{
    return rect;
}
