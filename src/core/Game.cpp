//
// Created by darwin on 18/04/26.
//
#include <core/Game.h>
#include <vector>
#include "entities/Obstacle.h"

// Colores Neón
const Color NEO_CYAN    = { 0, 255, 255, 255 };
const Color NEO_MAGENTA = { 255, 0, 255, 255 };
const Color NEO_YELLOW  = { 253, 249, 0, 255 };
const Color NEO_RED     = { 230, 41, 55, 255 };

// Constructor: Inicializamos variables base
Game::Game() {
    globalSpeed = 5.0f;
    speedIncrement = 0.0005f; // Incremento sutil por frame
    currentScreen = LOGIN;
    creditos = 100;
}

void Game::resetGame() {
    // Reiniciar al jugador
    player = Player();

    // Reiniciar dificultad
    globalSpeed = 5.0f;

    // Reiniciar obstáculos con distancias variadas (Tu rol - Parte 3)
    obstacles.clear();
    // Colocamos los obstáculos a distancias que den tiempo de reaccionar
    obstacles.push_back(Obstacle(1000, 310, 30, 45, globalSpeed)); 
    obstacles.push_back(Obstacle(1500, 220, 40, 25, globalSpeed)); 
}

void Game::run() {
    InitWindow(screenWidth, screenHeight, "Cyber-Runner: Neo-Guate");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- LÓGICA ---
        switch (currentScreen) {
            case LOGIN:
                if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
                break;

            case MENU:
                if (IsKeyPressed(KEY_P)) {
                    if (creditos >= 10) {
                        creditos -= 10;
                        resetGame(); 
                        currentScreen = JUGANDO;
                    }
                }
                break;

            case JUGANDO:
                // 1. Aumentar dificultad progresivamente
                globalSpeed += speedIncrement;

                // 2. Actualizar Jugador
                player.update();

                // 3. Actualizar Obstáculos con la nueva velocidad
                for (auto &obs : obstacles) {
                    obs.setSpeed(globalSpeed); // Sincroniza la velocidad actual
                    obs.update();
                }

                // 4. Detección de Colisiones (Física)
                for (auto &obs : obstacles) {
                    if (CheckCollisionRecs(player.getRect(), obs.getRect())) {
                        currentScreen = GAMEOVER;
                    }
                }

                if (IsKeyPressed(KEY_X)) currentScreen = GAMEOVER;
                break;

            case GAMEOVER:
                if (IsKeyPressed(KEY_R)) currentScreen = MENU;
                break;
        }

        // --- DIBUJADO ---
        BeginDrawing();
        ClearBackground(BLACK);

        switch (currentScreen) {
            case LOGIN:
                DrawText("CYBER-RUNNER: NEO-GUATE", 150, 150, 30, NEO_CYAN);
                DrawText("Presiona ENTER para Iniciar Sesion", 200, 250, 20, WHITE);
                break;

            case MENU:
                DrawText("MENU PRINCIPAL", 300, 50, 20, NEO_MAGENTA);
                DrawText(TextFormat("CREDITOS: %i", creditos), 320, 150, 20, NEO_YELLOW);
                DrawText("Presiona [P] para Jugar (Costo 10)", 220, 250, 20, WHITE);
                break;

            case JUGANDO:
                player.draw();
                for (auto &obs : obstacles) {
                    obs.draw();
                }

                // Piso Neón
                DrawLine(0, 350, 800, 350, NEO_MAGENTA);

                // UI de Juego
                DrawText(TextFormat("VELOCIDAD: %.2f", globalSpeed), 10, 10, 20, NEO_CYAN);
                DrawText("ESQUIVA LOS OBSTACULOS", 10, 40, 15, WHITE);
                break;

            case GAMEOVER:
                DrawText("SISTEMA CRITICO: GAME OVER", 180, 150, 30, NEO_RED);
                DrawText("Presiona [R] para volver al menu", 250, 250, 20, WHITE);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
}
