#pragma once
#include "raylib.h"

enum class ItemType {
    CREDIT,
    NITRO,
    SHIELD
};

// SECCION: Monedas
// Representa monedas y poderes recolectables que se mueven con el escenario.
class Coin {
private:
    // Hitbox y posicion visual del item.
    Rectangle rect;
    // Velocidad sincronizada con Game.
    float speed;
    // active=false evita dibujar y detectar colision despues de recoger o salir de pantalla.
    bool active;
    // Tipo del item: moneda, nitro o escudo.
    ItemType type;

    Texture2D coinTexture1 {};
    Texture2D coinTexture2 {};
    Texture2D nitroTexture {};
    Texture2D shieldTexture {};

    // Textura seleccionada segun tipo y frame de animacion.
    Texture2D* currentTexture = nullptr;

    // Animacion simple de monedas alternando dos sprites.
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

    // FUNCION: Mueve el item y actualiza su textura.
    void update(float deltaTime);
    // FUNCION: Dibuja el item si esta activo.
    void draw();
    // FUNCION: Reaparece con tipo aleatorio basico.
    void reset(float x);
    // FUNCION: Reaparece con posicion y tipo especificos.
    void reset(float x, float y, ItemType newType);
    // FUNCION: Sincroniza velocidad con el gameplay.
    void setSpeed(float newSpeed);

    // FUNCION: Devuelve hitbox y centro usados por colisiones/efectos.
    Rectangle getRect();
    Vector2 getPosition();

    // FUNCION: Estado de recoleccion.
    bool isActive();
    void collect();

    ItemType getType();
};
