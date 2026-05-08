#include "entities/Coin.h"

Coin::Coin(float x, float y, float size, float speed, ItemType type)
{
    rect = {x, y, size, size};
    this->speed = speed;
    this->type = type;
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
    if (!active)
    {
        return;
    }

    Color itemColor = GOLD;

    if (type == ItemType::NITRO)
    {
        itemColor = SKYBLUE;
    }
    else if (type == ItemType::SHIELD)
    {
        itemColor = ORANGE;
    }

    DrawCircle(
        rect.x + rect.width / 2,
        rect.y + rect.height / 2,
        rect.width / 2,
        itemColor
    );

    DrawCircleLines(
        rect.x + rect.width / 2,
        rect.y + rect.height / 2,
        rect.width / 2,
        WHITE
    );
}

void Coin::reset(float x)
{
    rect.x = x;
    rect.y = GetRandomValue(180, 300);
    active = true;

    int roll = GetRandomValue(0, 100);

    if (roll >= 90)
    {
        type = ItemType::NITRO;
    }
    else if (roll >= 80)
    {
        type = ItemType::SHIELD;
    }
    else
    {
        type = ItemType::CREDIT;
    }
}

void Coin::setSpeed(float newSpeed)
{
    speed = newSpeed;
}

Rectangle Coin::getRect()
{
    return rect;
}

Vector2 Coin::getPosition()
{
    return {
        rect.x + rect.width / 2,
        rect.y + rect.height / 2
    };
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

ItemType Coin::getType()
{
    return type;
}