#include "raylib.h"
#include <iostream>

// --- DEFINICION DE COLORES CYBERPUNK (Para evitar errores de scope) ---
// Formato: { Rojo, Verde, Azul, Alpha }
const Color NEO_CYAN    = { 0, 255, 255, 255 };
const Color NEO_MAGENTA = { 255, 0, 255, 255 };
const Color NEO_YELLOW  = { 253, 249, 0, 255 };
const Color NEO_RED     = { 230, 41, 55, 255 };

// Enumeración para controlar los estados del juego
typedef enum GameScreen { LOGIN, MENU, JUGANDO, GAMEOVER } GameScreen;

int main() {
    // 1. Inicialización de la ventana
    const int screenWidth = 800;
    const int screenHeight = 450;

    // Configuración para que la ventana se vea bien en laptops
    InitWindow(screenWidth, screenHeight, "Cyber-Runner: Neo-Guate");
    SetTargetFPS(60);

    GameScreen currentScreen = LOGIN;
    int creditos = 30; // Este valor lo manejará el Integrante D después

    // Variables temporales para el Robot (Tarea del Integrante B)
    Rectangle robot = { 100, 300, 50, 50 };
    float velocidadY = 0;
    bool enSuelo = true;

    // 2. Bucle Principal del Juego
    while (!WindowShouldClose()) {

        // --- LÓGICA / ACTUALIZACIÓN ---
        switch (currentScreen) {
            case LOGIN:
                if (IsKeyPressed(KEY_ENTER)) currentScreen = MENU;
                break;

            case MENU:
                if (IsKeyPressed(KEY_P)) {
                    if (creditos >= 10) {
                        creditos -= 10;
                        currentScreen = JUGANDO;
                    }
                }
                break;

            case JUGANDO:
                // Lógica de Salto y Gravedad (Base para el Integrante B)
                if (IsKeyPressed(KEY_SPACE) && enSuelo) {
                    velocidadY = -12.0f;
                    enSuelo = false;
                }

                velocidadY += 0.6f; // Gravedad constante
                robot.y += velocidadY;

                if (robot.y >= 300) { // Límite del "suelo"
                    robot.y = 300;
                    enSuelo = true;
                }

                // Simulación de muerte temporal para probar el GAMEOVER
                if (IsKeyPressed(KEY_X)) currentScreen = GAMEOVER;
                break;

            case GAMEOVER:
                if (IsKeyPressed(KEY_R)) currentScreen = MENU;
                break;
        }

        // --- DIBUJADO (RENDERIZADO) ---
        BeginDrawing();
            ClearBackground(BLACK); // Fondo negro base para estética Cyberpunk

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
                    // Dibujamos el robot (Caja cian)
                    DrawRectangleRec(robot, NEO_CYAN);

                    // Suelo neón magenta
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

    // 3. Finalizaciónn
    CloseWindow();
    return 0;
}
