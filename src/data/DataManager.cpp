#include "data/DataManager.h"

#include <fstream>

PlayerData DataManager::loadPlayerData()
{
    PlayerData data;

    std::ifstream file(saveFilePath);

    if (!file.is_open())
    {
        savePlayerData(data);
        return data;
    }

    file >> data.userId;
    file >> data.username;
    file >> data.credits;
    file >> data.highScore;
    file >> data.totalCoinsCollected;
    file >> data.gamesPlayed;

    file.close();

    return data;
}

void DataManager::savePlayerData(const PlayerData& data)
{
    std::ofstream file(saveFilePath);

    if (!file.is_open())
    {
        return;
    }

    file << data.userId << "\n";
    file << data.username << "\n";
    file << data.credits << "\n";
    file << data.highScore << "\n";
    file << data.totalCoinsCollected << "\n";
    file << data.gamesPlayed << "\n";

    file.close();
}

void DataManager::registerGameStarted(int userId)
{
    // Futuro: registrar inicio de partida en la base de datos.
    // Ejemplo: INSERT INTO game_sessions(user_id, status) VALUES(...)
}

void DataManager::registerGameFinished(int userId, int score, int coinsCollected)
{
    // Futuro: registrar resultado de partida en la base de datos.
    // Ejemplo: INSERT INTO game_sessions(user_id, score, coins_collected) VALUES(...)
}

void DataManager::registerCoinCollected(int userId, int amount)
{
    // Futuro: registrar transacción de monedas en la base de datos.
    // Ejemplo: INSERT INTO coin_transactions(user_id, amount, type) VALUES(...)
}