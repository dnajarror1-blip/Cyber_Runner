#pragma once
#include <string>

struct PlayerData {
    int userId = 1;
    std::string username = "Jugador";
    int credits = 15;
    int highScore = 0;
    int totalCoinsCollected = 0;
    int gamesPlayed = 0;
};