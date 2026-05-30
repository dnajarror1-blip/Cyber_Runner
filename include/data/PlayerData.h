#pragma once
#include <string>

// SECCION: Datos locales
// Estructura simple que DataManager lee y escribe en save_data.txt.
// La API sigue siendo la fuente principal para sesion, partidas y ranking.
struct PlayerData {
    // Valores por defecto usados cuando no existe archivo local.
    int userId = 1;
    std::string username = "Jugador";
    int credits = 15;
    int highScore = 0;
    int totalCoinsCollected = 0;
    int gamesPlayed = 0;
};