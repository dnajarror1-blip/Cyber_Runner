#pragma once
#include "raylib.h"

class Coin {
private:
    Rectangle rect;
    float speed;
    bool active;

public:
    Coin(float x, float y, float size, float speed);

    void update(float deltaTime);
    void draw();
    void reset(float x);
    void setSpeed(float newSpeed);

    Rectangle getRect();
    bool isActive();
    void collect();
};