#pragma once
#include "raylib.h"

enum class ItemType {
    CREDIT,
    NITRO,
    SHIELD
};

class Coin {
private:
    Rectangle rect;
    float speed;
    bool active;
    ItemType type;

public:
    Coin(float x, float y, float size, float speed, ItemType type = ItemType::CREDIT);

    void update(float deltaTime);
    void draw();
    void reset(float x);
    void setSpeed(float newSpeed);

    Rectangle getRect();
    Vector2 getPosition();

    bool isActive();
    void collect();

    ItemType getType();
};