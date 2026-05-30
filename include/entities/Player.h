#pragma once
#include "raylib.h"

// SECCION: Jugador
// Representa al corredor: posicion, fisica de salto, animacion y eventos de audio.
class Player {
private:

    // Hitbox principal del jugador. x/y ubican al jugador; width/height definen colisiones.
    Rectangle rect = {100, 292, 28, 58};

    // Velocidad vertical usada por gravedad, salto y caida rapida.
    float velocidadY = 0.0f;

    // Altura del suelo para este hitbox. Si se cambia el piso visual, revisar este valor.
    float sueloY = 292.0f;

    // Valores de fisica del salto. No cambiarlos sin probar colisiones y dificultad.
    float gravedad = 900.0f;
    float fuerzaSalto = -400.0f;
    float fuerzaSaltoNitro = -550.0f;

    // Limites de caida normal y caida rapida.
    float velocidadCaidaMaxima = 600.0f;
    float fastFallSpeed = 1000.0f;

    // Estado de salto: permite salto inicial y doble salto.
    bool enSuelo = true;
    int saltosDisponibles = 2;

    // El nitro modifica el segundo salto.
    bool hasNitro = false;

    // Eventos de un frame usados por AudioManager desde Game.
    bool jumpEvent = false;

    bool doubleJumpEvent = false;

    bool landingEvent = false;

    bool wasInAir = false;

    Texture2D run1 {};
    Texture2D run2 {};
    Texture2D run3 {};
    Texture2D jump {};
    Texture2D jumpBoost {};

    // Textura que se dibuja en el frame actual.
    Texture2D* currentTexture = nullptr;

    // Control de animacion de carrera.
    int frameActual = 0;

    float frameTime = 0.0f;
    float frameSpeed = 0.10f;

public:

    Player();
    ~Player();

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    Player(Player&& other) noexcept = default;
    Player& operator=(Player&& other) noexcept = default;

    // FUNCION: Actualiza controles, fisica, eventos y animacion.
    void update(float deltaTime);
    // FUNCION: Dibuja el sprite actual o el rectangulo de respaldo.
    void draw();

    // FUNCION: Devuelve el rectangulo usado para colisiones.
    Rectangle getRect();

    // FUNCION: Devuelve el centro del jugador para efectos visuales.
    Vector2 getPosition();

    // FUNCION: Activa o desactiva el estado nitro en el jugador.
    void setNitro(bool active);

    bool isNitroActive() const;

    // FUNCION: Eventos leidos por Game para reproducir sonidos.
    bool detectJumpStart();

    bool detectDoubleJump();

    bool detectLanding();

    bool isGrounded() const;

};