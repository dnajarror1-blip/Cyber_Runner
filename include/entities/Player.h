//
// Created by darwin on 18/04/26.
//

#pragma once
#include "raylib.h"

class Player {
private:
    Rectangle rect = { 100, 300, 50, 50 };
    float velocidadY = 0;
    bool enSuelo = true;

public:
    void update();
    void draw();
    Rectangle getRect();

};

#ifndef CYBER_RUNNER_PLAYER_H
#define CYBER_RUNNER_PLAYER_H

#endif //CYBER_RUNNER_PLAYER_H
