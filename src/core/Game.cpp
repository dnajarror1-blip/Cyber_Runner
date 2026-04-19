//
// Created by darwin on 18/04/26.
//
#include <core/Game.h>
#include <vector>
#include "entities/Obstacle.h"

// Colores
const Color NEO_CYAN    = { 0, 255, 255, 255 };
const Color NEO_MAGENTA = { 255, 0, 255, 255 };
const Color NEO_YELLOW  = { 253, 249, 0, 255 };
const Color NEO_RED     = { 230, 41, 55, 255 };


void Game::resetGame() {
    //reiniciar al jugador
    player = Player();

    //reiniciar obstaculos
    obstacles.clear();
    obstacles.push_back(Obstacle(1000, 300, 40, 50, 5));
    obstacles.push_back(Obstacle(1400, 300, 40, 50, 5));
}

void Game::run() {

    InitWindow(screenWidth, screenHeight, "Cyber-Runner: Neo-Guate");
    SetTargetFPS(60);


    while (!WindowShouldClose()) {

        // --- LOGICA ---
        switch (currentScreen) {
            case LOGIN:
                if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
                break;

            case MENU:
                if (IsKeyPressed(KEY_P)) {
                    if (creditos >= 10) {
                        creditos -= 10;
                        resetGame(); //  AQUÍ
                        currentScreen = JUGANDO;
                    }
                }
                break;

            case JUGANDO:
                player.update();
                for (auto &obs : obstacles) {
                    obs.update();
                }
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

                DrawLine(0, 350, 800, 350, NEO_MAGENTA);

                DrawText("ESQUIVA LOS OBSTACULOS", 10, 10, 20, WHITE);
                DrawText("Presiona [X] para simular perder", 10, 40, 15, GRAY);
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