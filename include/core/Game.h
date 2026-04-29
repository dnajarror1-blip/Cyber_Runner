//
// Created by darwin on 18/04/26.
//

#ifndef CYBER_RUNNER_GAME_H
#define CYBER_RUNNER_GAME_H

#pragma once
#include "entities/Player.h"
#include "entities/Obstacle.h"
#include <vector>
#include <string>

// Definición de los estados del juego
enum GameScreen { LOGIN, MENU, JUGANDO, GAMEOVER };

class Game {
private:
    // --- Configuración de Ventana ---
    const int screenWidth = 800;
    const int screenHeight = 450;

    // --- Lógica de Obstáculos (Tu Rol - Parte 3) ---
    std::vector<Obstacle> obstacles;
    float globalSpeed;      // Controla la dificultad progresiva
    float speedIncrement;   // Qué tan rápido aumenta la dificultad

    // --- Entidades ---
    Player player;

    // --- Estado y Economía ---
    GameScreen currentScreen;
    int creditos;
    std::string playerName; // Para el sistema de guardado del Integrante D

    // --- Métodos Privados de Soporte ---
    void updateGame();      // Lógica de movimiento y colisiones
    void drawGame();        // Dibujado de todas las entidades
    void checkCollisions(); // Colaboración con el Integrante 2 (Física)

public:
    // Constructor y Métodos principales
    Game();
    void run();             // El bucle principal del juego
    void resetGame();       // Reinicia el nivel, obstáculos y velocidad
};

#endif // CYBER_RUNNER_GAME_H


#ifndef CYBER_RUNNER_GAME_H
#define CYBER_RUNNER_GAME_H

#endif //CYBER_RUNNER_GAME_H
