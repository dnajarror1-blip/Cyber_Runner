#include "entities/Coin.h"

Coin::Coin(float x, float y, float size, float speed)
{
    rect = {x, y, size, size};
    this->speed = speed;
    active = true;
}

void Coin::update(float deltaTime)
{
    rect.x -= speed * deltaTime;

    if (rect.x + rect.width < 0)
    {
        reset(800 + GetRandomValue(200, 600));
    }
}

void Coin::draw()
{
    if (active)
    {
        DrawCircle(rect.x + rect.width / 2, rect.y + rect.height / 2, rect.width / 2, GOLD);
        DrawCircleLines(rect.x + rect.width / 2, rect.y + rect.height / 2, rect.width / 2, ORANGE);
    }
}

void Coin::reset(float x)
{
    rect.x = x;
    rect.y = GetRandomValue(180, 300);
    active = true;
}

void Coin::setSpeed(float newSpeed)
{
    speed = newSpeed;
}

Rectangle Coin::getRect()
{
    return rect;
}

bool Coin::isActive()
{
    return active;
}

void Coin::collect()
{
    active = false;
    reset(800 + GetRandomValue(300, 800));
}