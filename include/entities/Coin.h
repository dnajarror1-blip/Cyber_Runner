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

    Texture2D coinTexture1 {};
    Texture2D coinTexture2 {};
    Texture2D nitroTexture {};

    Texture2D* currentTexture = nullptr;

    float animationTimer = 0.0f;
    float animationSpeed = 0.12f;

    bool animationFrame = false;

public:
    Coin(float x, float y, float size, float speed, ItemType type = ItemType::CREDIT);

    ~Coin();

    Coin(const Coin&) = delete;
    Coin& operator=(const Coin&) = delete;

    Coin(Coin&& other) noexcept;
    Coin& operator=(Coin&& other) noexcept;

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