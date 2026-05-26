#include "entities/Player.h"

Player::Player() {
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
    jumpEvent = false;

    doubleJumpEvent = false;

    landingEvent = false;

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

    velocidadY += gravedad * deltaTime;

    if (velocidadY > velocidadCaidaMaxima)
    {
        velocidadY = velocidadCaidaMaxima;
    }

    rect.y += velocidadY * deltaTime;

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
