//
// Created by darwin on 18/04/26.
//

#pragma once
#include "raylib.h"

class Player {
private:

    // HITBOX
    Rectangle rect = { 100, 324, 36, 26 };

    // FISICAS
    float velocidadY = 0.0f;
    float sueloY = 324.0f;

    float gravedad = 900.0f;
    float fuerzaSalto = -400.0f;
    float fuerzaSaltoNitro = -550.0f;
    float velocidadCaidaMaxima = 600.0f;
    float fastFallSpeed = 1000.0f;

    bool enSuelo = true;
    int saltosDisponibles = 2;

    bool hasNitro = false;

    // TEXTURA
    Texture2D texture = {0};

    // ANIMACION
    Rectangle frameRec = {0};

    int frameActual = 0;
    int totalFrames = 4;

    float frameTime = 0.0f;
    float frameSpeed = 0.10f;

    // Cada frame completo del spritesheet mide 384x1024,
    // pero solo recortaremos la zona donde está el robot.
    float frameWidth = 0.0f;
    float frameHeight = 0.0f;

    float spriteCropY = 330.0f;
    float spriteCropHeight = 460.0f;

    // VISUAL
    float visualScale = 2.2f;
    float hitboxOffsetY = 0.0f;

public:

    Player();
    ~Player();

    // CRITICO:
    // PROHIBIR COPIAS PARA EVITAR DOUBLE FREE DE Texture2D

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    // MOVIMIENTO SEGURO

    Player(Player&& other) noexcept = default;
    Player& operator=(Player&& other) noexcept = default;

    void update(float deltaTime);
    void draw();

    Rectangle getRect();
    Vector2 getPosition();

    void setNitro(bool active);
    bool isNitroActive() const;
};