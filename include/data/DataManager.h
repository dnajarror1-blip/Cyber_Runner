#pragma once

#include "data/PlayerData.h"

class DataManager {
private:
    const char* saveFilePath = "save_data.txt";

public:
    PlayerData loadPlayerData();
    void savePlayerData(const PlayerData& data);

    void registerGameStarted(int userId);
    void registerGameFinished(int userId, int score, int coinsCollected);
    void registerCoinCollected(int userId, int amount);
};