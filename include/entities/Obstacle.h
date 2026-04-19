//
// Created by darwin on 18/04/26.
//
#pragma once
#include "raylib.h"

class Obstacle {
private:
    Rectangle rect;
    float speed;

    public:
    Obstacle(float x, float y, float width, float height, float speed);

    void update();
    void draw();

    Rectangle getRect();

};

#ifndef CYBER_RUNNER_OBSTACLE_H
#define CYBER_RUNNER_OBSTACLE_H

#endif //CYBER_RUNNER_OBSTACLE_H
