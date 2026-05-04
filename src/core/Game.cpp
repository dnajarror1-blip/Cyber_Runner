//
// Created by darwin on 18/04/26.
//
#include <core/Game.h>
#include <vector>
#include <algorithm>
#include "entities/Obstacle.h"

// Paleta de Colores Neón
const Color NEO_CYAN = {0, 255, 255, 255};
const Color NEO_MAGENTA = {255, 0, 255, 255};
const Color NEO_YELLOW = {253, 249, 0, 255};
const Color NEO_RED = {230, 41, 55, 255};

// Constructor: Inicialización del sistema
Game::Game()
{
    globalSpeed = 350.0f;
    speedIncrement = 30.0f;
    currentScreen = LOGIN;
    creditos = 100;
}

void Game::resetGame()
{
    // Reiniciar al jugador
    player = Player();

    // Reiniciar dificultad base
    globalSpeed = 350.0f;

    // Reiniciar vector de obstáculos (Tu Rol - Parte 3)
    obstacles.clear();
    // Los posicionamos a distancias estratégicas
    obstacles.push_back(Obstacle(1000, 310, 30, 45, globalSpeed));
    obstacles.push_back(Obstacle(1500, 220, 40, 25, globalSpeed));
}

void Game::run()
{
    InitWindow(screenWidth, screenHeight, "Cyber-Runner");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_F11)) {
            if (!IsWindowFullscreen()) {
                int monitor = GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
                ToggleFullscreen();
            } else {
                ToggleFullscreen();
                SetWindowSize(screenWidth, screenHeight);
            };
        }
        // --- LÓGICA DE CONTROL ---
        switch (currentScreen)
        {
        case LOGIN:
            if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
            break;

        case MENU:
            // Opción 1: Iniciar partida
            if (IsKeyPressed(KEY_ONE))
            {
                if (creditos >= 5)
                {
                    creditos -= 5;
                    resetGame();
                    currentScreen = JUGANDO;
                }
            }
            // Opción 4: Salir del programa
            if (IsKeyPressed(KEY_FOUR))
            {
                CloseWindow();
                return; // Finaliza la ejecución
            }
            // Opciones 2 y 3: Login y Carga (Pendientes Integrante D)
            break;

        case JUGANDO:
            {
                float deltaTime = GetFrameTime();

                globalSpeed += speedIncrement * deltaTime;

                if (globalSpeed > 900.0f)
                {
                    globalSpeed = 900.0f;
                }

                player.update(deltaTime);

                for (auto& obs : obstacles)
                {
                    obs.setSpeed(globalSpeed);
                    obs.update(deltaTime);
                }

                for (auto& obs : obstacles)
                {
                    if (CheckCollisionRecs(player.getRect(), obs.getRect()))
                    {
                        currentScreen = GAMEOVER;
                    }
                }

                break;
            }
        case GAMEOVER:
            if (IsKeyPressed(KEY_R)) currentScreen = MENU;
            break;
        }

        // --- DIBUJADO ---
        BeginTextureMode(target);
        ClearBackground(BLACK);

        switch (currentScreen)
        {
        case LOGIN:
            DrawText("CYBER-RUNNER", 250, 80, 35, NEO_CYAN);
            DrawText("[ Presiona ENTER para entrar al sistema ]", 180, 350, 20, GRAY);
            break;

        case MENU:
            DrawText("ACCESO CONCEDIDO", 300, 40, 20, GREEN);
            DrawText("--- TERMINAL DE CONTROL ---", 220, 80, 25, NEO_MAGENTA);

            DrawRectangleLines(250, 140, 300, 40, NEO_CYAN);
            DrawText("[1] EMPEZAR PARTIDA", 280, 150, 20, WHITE);

            DrawRectangleLines(250, 200, 300, 40, WHITE);
            DrawText("[2] LOGUEARSE", 280, 210, 20, GRAY);

            DrawRectangleLines(250, 260, 300, 40, WHITE);
            DrawText("[3] CARGAR USUARIO", 280, 270, 20, GRAY);

            DrawRectangleLines(250, 320, 300, 40, NEO_RED);
            DrawText("[4] SALIR DEL JUEGO", 280, 330, 20, WHITE);

            DrawText(TextFormat("CREDITOS: %i", creditos), 280, 400, 15, NEO_YELLOW);
            break;

        case JUGANDO:
            player.draw();

            for (auto& obs : obstacles)
            {
                obs.draw();
            }

            DrawLine(0, 350, 800, 350, NEO_MAGENTA);
            DrawText(TextFormat("VELOCIDAD: %.2f", globalSpeed), 10, 10, 20, NEO_CYAN);
            DrawText(TextFormat("INCREMENTO: %.2f", speedIncrement), 10, 35, 20, NEO_YELLOW);
            break;

        case GAMEOVER:
            DrawText("SISTEMA CRITICO: GAME OVER", 180, 150, 30, NEO_RED);
            DrawText("Presiona [R] para volver al menu", 250, 250, 20, WHITE);
            break;
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        float scale = std::min(
            (float)GetScreenWidth() / screenWidth,
            (float)GetScreenHeight() / screenHeight
        );

        float scaledWidth = screenWidth * scale;
        float scaledHeight = screenHeight * scale;

        float offsetX = (GetScreenWidth() - scaledWidth) / 2.0f;
        float offsetY = (GetScreenHeight() - scaledHeight) / 2.0f;

        Rectangle source = {
            0.0f,
            0.0f,
            (float)target.texture.width,
            -(float)target.texture.height
        };

        Rectangle dest = {
            offsetX,
            offsetY,
            scaledWidth,
            scaledHeight
        };

        DrawTexturePro(target.texture, source, dest, {0, 0}, 0.0f, WHITE);

        EndDrawing();
    }
    CloseWindow();
}
