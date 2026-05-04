//
// Created by darwin on 18/04/26.
//

#pragma once
#include "raylib.h"

class Player {
private:
    Rectangle rect = { 100, 300, 50, 50 };

    float velocidadY = 0.0f;
    float sueloY = 300.0f;
    float gravedad = 1800.0f;
    float fuerzaSalto = -700.0f;
    float velocidadCaidaMaxima = 1200.0f;

    bool enSuelo = true;
    int saltosDisponibles = 2;

public:
    void update(float deltaTime);
    void draw();
    Rectangle getRect();

};
