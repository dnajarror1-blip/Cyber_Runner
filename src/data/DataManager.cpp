#include "data/DataManager.h"

#include <fstream>

PlayerData DataManager::loadPlayerData()
{
    // SECCION: Datos locales
    // Carga respaldo local usado por menus, record y tokens visibles.
    PlayerData data;

    std::ifstream file(saveFilePath);

    // Si no existe archivo, se crea uno con los valores por defecto de PlayerData.
    if (!file.is_open())
    {
        savePlayerData(data);
        return data;
    }

    // IMPORTANTE: El orden de lectura debe coincidir con savePlayerData().
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
    // Guarda el respaldo local en texto plano para que sea simple de revisar.
    std::ofstream file(saveFilePath);

    if (!file.is_open())
    {
        return;
    }

    // IMPORTANTE: Si se agrega un campo, actualizar tambien loadPlayerData().
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
    // Punto de extension local; actualmente la partida real se maneja por API.
    // Futuro: registrar inicio de partida en la base de datos.
    // Ejemplo: INSERT INTO game_sessions(user_id, status) VALUES(...)
}

void DataManager::registerGameFinished(int userId, int score, int coinsCollected)
{
    // Punto de extension local para guardar resumen historico si se necesita.
    // Futuro: registrar resultado de partida en la base de datos.
    // Ejemplo: INSERT INTO game_sessions(user_id, score, coins_collected) VALUES(...)
}

void DataManager::registerCoinCollected(int userId, int amount)
{
    // Punto de extension local para auditar recolecciones si se necesita.
    // Futuro: registrar transacción de monedas en la base de datos.
    // Ejemplo: INSERT INTO coin_transactions(user_id, amount, type) VALUES(...)
}