//
// Created by darwin on 18/04/26.
//

#include "entities/Player.h"

Player::Player()
{
    // Cargar sprite del robot
    texture = LoadTexture("assets/player.png");

    if (texture.id == 0)
    {
        TraceLog(LOG_ERROR, "No se pudo cargar assets/player.png");
        return;
    }

    TraceLog(LOG_INFO, "player.png cargado correctamente");

    frameWidth =
        static_cast<float>(texture.width) /
        static_cast<float>(totalFrames);

    // No usamos todo el alto de la imagen porque tiene mucho espacio vacío.
    frameHeight = spriteCropHeight;

    frameRec = {
        0.0f,
        spriteCropY,
        frameWidth,
        frameHeight
    };

    hasNitro = false;
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
        if (hasNitro && saltosDisponibles == 1)
        {
            velocidadY = fuerzaSaltoNitro;
        }
        else
        {
            velocidadY = fuerzaSalto;
        }

        saltosDisponibles--;

        enSuelo = false;
    }

    if (!enSuelo && IsKeyDown(KEY_DOWN))
    {
        velocidadY = fastFallSpeed;
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

    frameRec.y = spriteCropY;
    frameRec.width = frameWidth;
    frameRec.height = spriteCropHeight;
}

void Player::draw()
{
    // Fallback seguro si falla el asset
    if (texture.id == 0)
    {
        DrawRectangleRec(rect, {0, 255, 255, 255});
        return;
    }

    if (hasNitro)
    {
        DrawCircleLines(
            rect.x + rect.width / 2,
            rect.y + rect.height / 2,
            35,
            SKYBLUE
        );

        DrawCircleLines(
            rect.x + rect.width / 2,
            rect.y + rect.height / 2,
            38,
            Fade(SKYBLUE, 0.3f)
        );
    }

    // TAMAÑO VISUAL DESACOPLADO DE HITBOX
    float drawWidth = 110.0f;

    float drawHeight = 130.0f;

    // CENTRADO VISUAL
    float offsetX =
        (drawWidth - rect.width) / 2.0f;

    // AJUSTE CRITICO VISUAL
    float offsetY = drawHeight - rect.height;

    Rectangle dest = {
        rect.x - offsetX,
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

Vector2 Player::getPosition()
{
    return {
        rect.x + rect.width / 2,
        rect.y + rect.height / 2
    };
}

void Player::setNitro(bool active)
{
    hasNitro = active;
}

bool Player::isNitroActive() const
{
    return hasNitro;
}
