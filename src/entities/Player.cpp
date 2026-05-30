#include "entities/Player.h"

Player::Player() {
    // SECCION: Assets del jugador
    // Estas texturas controlan la animacion de correr, salto y doble salto con nitro.
    run1 = LoadTexture("assets/player1.png");
    run2 = LoadTexture("assets/player2.png");
    run3 = LoadTexture("assets/player3.png");
    jump = LoadTexture("assets/player4.png");
    jumpBoost = LoadTexture("assets/playersal1.png");

    currentTexture = &run1;
}

Player::~Player() {
    if (run1.id) UnloadTexture(run1);
    if (run2.id) UnloadTexture(run2);
    if (run3.id) UnloadTexture(run3);
    if (jump.id) UnloadTexture(jump);
    if (jumpBoost.id) UnloadTexture(jumpBoost);
}

void Player::update(float deltaTime)
{
    // Los eventos duran un frame; Game los consulta para reproducir audio.
    jumpEvent = false;

    doubleJumpEvent = false;

    landingEvent = false;

    // SECCION: Entrada con teclado y control
    // SPACE o A del control activan salto si quedan saltos disponibles.
    bool jumpPressed =
            IsKeyPressed(KEY_SPACE) ||
            (
                IsGamepadAvailable(0) &&
                IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
            );

    if (jumpPressed && saltosDisponibles > 0)
    {
        bool firstJump = saltosDisponibles == 2;

        bool secondJump = saltosDisponibles == 1;

        // El segundo salto usa fuerza especial cuando nitro esta activo.
        velocidadY =
                (hasNitro && secondJump)
                    ? fuerzaSaltoNitro
                    : fuerzaSalto;

        saltosDisponibles--;

        enSuelo = false;

        wasInAir = true;

        if (firstJump)
        {
            jumpEvent = true;
        }

        if (secondJump)
        {
            doubleJumpEvent = true;
        }
    }

    // Caida rapida: flecha abajo, direccion abajo o boton inferior izquierdo del control.
    bool fastFallPressed =
            IsKeyDown(KEY_DOWN) ||
            (
                IsGamepadAvailable(0) &&
                (
                    IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
                    GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y) > 0.5f
                )
            );

    if (!enSuelo && fastFallPressed)
    {
        velocidadY = fastFallSpeed;
    }

    // SECCION: Fisica del jugador
    // La gravedad aumenta velocidadY y luego se aplica a la posicion vertical.
    velocidadY += gravedad * deltaTime;

    if (velocidadY > velocidadCaidaMaxima)
    {
        velocidadY = velocidadCaidaMaxima;
    }

    rect.y += velocidadY * deltaTime;

    // Al tocar suelo se reinician saltos y se marca aterrizaje si venia en el aire.
    if (rect.y >= sueloY)
    {
        rect.y = sueloY;

        velocidadY = 0.0f;

        if (!enSuelo && wasInAir)
        {
            landingEvent = true;
        }

        enSuelo = true;

        wasInAir = false;

        saltosDisponibles = 2;
    }

    // SECCION: Animacion del jugador
    // En suelo se alternan frames de carrera; en aire se usan sprites de salto.
    if (enSuelo)
    {
        frameTime += deltaTime;

        if (frameTime >= frameSpeed)
        {
            frameTime = 0.0f;

            frameActual++;

            if (frameActual > 2)
            {
                frameActual = 0;
            }
        }

        switch (frameActual)
        {
            case 0:
                currentTexture = &run1;
                break;

            case 1:
                currentTexture = &run2;
                break;

            case 2:
                currentTexture = &run3;
                break;
        }
    }
    else
    {
        if (saltosDisponibles == 0 && velocidadY < 0.0f)
        {
            currentTexture = &jumpBoost;
        }
        else
        {
            currentTexture = &jump;
        }
    }
}

void Player::draw() {
    // Si una textura no cargo, se dibuja el hitbox para que el juego siga siendo visible.
    if (currentTexture == nullptr || currentTexture->id == 0) {
        DrawRectangleRec(rect, RED);
        return;
    }

    Rectangle source = {
        0,
        0,
        (float) currentTexture->width,
        (float) currentTexture->height
    };

    // dest usa el mismo rectangulo que las colisiones del jugador.
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
        {0, 0},
        0.0f,
        WHITE
    );

    // SECCION: Hitbox visual
    // Descomentar este bloque ayuda a depurar el rectangulo real de colision.
    //DrawRectangleLinesEx(// Contorno del hitbox, se ha dejado como comment para cualquier cambio adicional
    //    rect,
    //    1.0f,
    //    SKYBLUE
    //
}

Rectangle Player::getRect() {
    return rect;
}

Vector2 Player::getPosition() {
    return {
        rect.x + rect.width / 2.0f,
        rect.y + rect.height / 2.0f
    };
}

void Player::setNitro(bool active) {
    hasNitro = active;
}

bool Player::isNitroActive() const {
    return hasNitro;
}
bool Player::detectJumpStart()
{
    return jumpEvent;
}

bool Player::detectDoubleJump()
{
    return doubleJumpEvent;
}

bool Player::detectLanding()
{
    return landingEvent;
}

bool Player::isGrounded() const
{
    return enSuelo;
}
