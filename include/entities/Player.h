#pragma once
#include "raylib.h"

class Player {
private:

    Rectangle rect = {100, 292, 28, 58};

    float velocidadY = 0.0f;

    float sueloY = 292.0f;

    float gravedad = 900.0f;
    float fuerzaSalto = -400.0f;
    float fuerzaSaltoNitro = -550.0f;

    float velocidadCaidaMaxima = 600.0f;
    float fastFallSpeed = 1000.0f;

    bool enSuelo = true;
    int saltosDisponibles = 2;

    bool hasNitro = false;

    Texture2D run1 {};
    Texture2D run2 {};
    Texture2D run3 {};
    Texture2D jump {};

    Texture2D* currentTexture = nullptr;

    int frameActual = 0;

    float frameTime = 0.0f;
    float frameSpeed = 0.10f;

    const float visualWidth = 42.0f;
    const float visualHeight = 68.0f;

public:

    Player();
    ~Player();

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    Player(Player&& other) noexcept = default;
    Player& operator=(Player&& other) noexcept = default;

    void update(float deltaTime);
    void draw();

    Rectangle getRect();

    Vector2 getPosition();

    void setNitro(bool active);

    bool isNitroActive() const;
};