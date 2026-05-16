#include "entities/Coin.h"

Coin::Coin(float x, float y, float size, float speed, ItemType type)
{
    rect = {x, y, size, size};

    this->speed = speed;
    this->type = type;

    active = true;

    coinTexture1 = LoadTexture("assets/coin1.png");
    coinTexture2 = LoadTexture("assets/coin2.png");
    nitroTexture = LoadTexture("assets/nitro.png");

    currentTexture = &coinTexture1;
}

Coin::~Coin()
{
    if (coinTexture1.id)
    {
        UnloadTexture(coinTexture1);
    }

    if (coinTexture2.id)
    {
        UnloadTexture(coinTexture2);
    }

    if (nitroTexture.id)
    {
        UnloadTexture(nitroTexture);
    }
}

Coin::Coin(Coin&& other) noexcept
{
    rect = other.rect;
    speed = other.speed;
    active = other.active;
    type = other.type;

    coinTexture1 = other.coinTexture1;
    coinTexture2 = other.coinTexture2;
    nitroTexture = other.nitroTexture;

    animationTimer = other.animationTimer;
    animationSpeed = other.animationSpeed;
    animationFrame = other.animationFrame;

    if (other.currentTexture == &other.coinTexture1)
    {
        currentTexture = &coinTexture1;
    }
    else if (other.currentTexture == &other.coinTexture2)
    {
        currentTexture = &coinTexture2;
    }
    else if (other.currentTexture == &other.nitroTexture)
    {
        currentTexture = &nitroTexture;
    }
    else
    {
        currentTexture = nullptr;
    }

    other.coinTexture1 = {};
    other.coinTexture2 = {};
    other.nitroTexture = {};
    other.currentTexture = nullptr;
}

Coin& Coin::operator=(Coin&& other) noexcept
{
    if (this != &other)
    {
        if (coinTexture1.id)
        {
            UnloadTexture(coinTexture1);
        }

        if (coinTexture2.id)
        {
            UnloadTexture(coinTexture2);
        }

        if (nitroTexture.id)
        {
            UnloadTexture(nitroTexture);
        }

        rect = other.rect;
        speed = other.speed;
        active = other.active;
        type = other.type;

        coinTexture1 = other.coinTexture1;
        coinTexture2 = other.coinTexture2;
        nitroTexture = other.nitroTexture;

        animationTimer = other.animationTimer;
        animationSpeed = other.animationSpeed;
        animationFrame = other.animationFrame;

        if (other.currentTexture == &other.coinTexture1)
        {
            currentTexture = &coinTexture1;
        }
        else if (other.currentTexture == &other.coinTexture2)
        {
            currentTexture = &coinTexture2;
        }
        else if (other.currentTexture == &other.nitroTexture)
        {
            currentTexture = &nitroTexture;
        }
        else
        {
            currentTexture = nullptr;
        }

        other.coinTexture1 = {};
        other.coinTexture2 = {};
        other.nitroTexture = {};
        other.currentTexture = nullptr;
    }

    return *this;
}

void Coin::update(float deltaTime)
{
    rect.x -= speed * deltaTime;

    animationTimer += deltaTime;

    if (animationTimer >= animationSpeed)
    {
        animationTimer = 0.0f;

        animationFrame = !animationFrame;
    }

    if (type == ItemType::CREDIT)
    {
        currentTexture =
            animationFrame
            ? &coinTexture1
            : &coinTexture2;
    }
    else if (type == ItemType::NITRO)
    {
        currentTexture = &nitroTexture;
    }
    else
    {
        currentTexture = nullptr;
    }

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

    if (type == ItemType::SHIELD)
    {
        DrawCircle(
            rect.x + rect.width / 2,
            rect.y + rect.height / 2,
            rect.width / 2,
            ORANGE
        );

        DrawCircleLines(
            rect.x + rect.width / 2,
            rect.y + rect.height / 2,
            rect.width / 2,
            WHITE
        );

        return;
    }

    if (currentTexture == nullptr || currentTexture->id == 0)
    {
        DrawRectangleRec(rect, RED);
        return;
    }

    Rectangle source = {
        0,
        0,
        (float)currentTexture->width,
        (float)currentTexture->height
    };

    Rectangle dest = {
        rect.x,
        rect.y,
        rect.width,
        rect.height
    };

    DrawTexturePro(
        *currentTexture,
        source,
        dest,
        {0,0},
        0.0f,
        WHITE
    );
}

void Coin::reset(float x)
{
    rect.x = x;
    rect.y = GetRandomValue(180, 300);

    active = true;

    animationTimer = 0.0f;

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