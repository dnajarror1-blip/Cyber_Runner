#include "entities/Coin.h"

Coin::Coin(float x, float y, float size, float speed, ItemType type)
{
    // SECCION: Monedas - posicion y tamano
    // size define ancho y alto del hitbox del item.
    rect = {x, y, size, size};

    this->speed = speed;
    this->type = type;

    active = true;

    // Texturas de monedas y poderes.
    coinTexture1 = LoadTexture("assets/coin1.png");
    coinTexture2 = LoadTexture("assets/coin2.png");
    nitroTexture = LoadTexture("assets/nitro.png");
    shieldTexture = LoadTexture("assets/escudo.png");

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

    if (shieldTexture.id)
    {
        UnloadTexture(shieldTexture);
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
    shieldTexture = other.shieldTexture;

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
    else if (other.currentTexture == &other.shieldTexture)
    {
        currentTexture = &shieldTexture;
    }
    else
    {
        currentTexture = nullptr;
    }

    other.coinTexture1 = {};
    other.coinTexture2 = {};
    other.nitroTexture = {};
    other.shieldTexture = {};
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

        if (shieldTexture.id)
        {
            UnloadTexture(shieldTexture);
        }

        rect = other.rect;
        speed = other.speed;
        active = other.active;
        type = other.type;

        coinTexture1 = other.coinTexture1;
        coinTexture2 = other.coinTexture2;
        nitroTexture = other.nitroTexture;
        shieldTexture = other.shieldTexture;

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
        else if (other.currentTexture == &other.shieldTexture)
        {
            currentTexture = &shieldTexture;
        }
        else
        {
            currentTexture = nullptr;
        }

        other.coinTexture1 = {};
        other.coinTexture2 = {};
        other.nitroTexture = {};
        other.shieldTexture = {};
        other.currentTexture = nullptr;
    }

    return *this;
}

void Coin::update(float deltaTime)
{
    // Si el item ya fue recogido o salio de pantalla, no se actualiza.
    if (!active)
    {
        return;
    }

    // Movimiento horizontal con la misma velocidad del escenario.
    rect.x -= speed * deltaTime;

    // SECCION: Animacion de item
    // Las monedas alternan dos texturas; los poderes usan textura fija.
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
    else if (type == ItemType::SHIELD)
    {
        currentTexture = &shieldTexture;
    }
    else
    {
        currentTexture = nullptr;
    }

    // Cuando sale por la izquierda se desactiva hasta que Game lo regenere.
    if (rect.x + rect.width < 0)
    {
        active = false;
    }
}

void Coin::draw()
{
    // Solo se dibujan items activos.
    if (!active)
    {
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

    // dest usa el mismo rectangulo que se revisa para colision.
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
    // SECCION: Respawn de monedas
    // Este reset decide si aparece escudo o moneda normal.
    int roll = GetRandomValue(0, 100);

    if (roll >= 80)
    {
        reset(x, GetRandomValue(180, 300), ItemType::SHIELD);
    }
    else
    {
        reset(x, GetRandomValue(180, 300), ItemType::CREDIT);
    }
}

void Coin::reset(float x, float y, ItemType newType)
{
    // Reposiciona y reactiva el item para reutilizar el mismo objeto.
    rect.x = x;
    rect.y = y;
    type = newType;
    active = true;
    animationTimer = 0.0f;

    // Selecciona textura inicial segun tipo.
    if (type == ItemType::SHIELD)
    {
        currentTexture = &shieldTexture;
    }
    else if (type == ItemType::NITRO)
    {
        currentTexture = &nitroTexture;
    }
    else
    {
        currentTexture = &coinTexture1;
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
    // La recoleccion solo desactiva; Game decide score, tokens y sonido.
    active = false;
}

ItemType Coin::getType()
{
    return type;
}
