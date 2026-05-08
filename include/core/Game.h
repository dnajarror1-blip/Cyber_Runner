#pragma once

#include "entities/Player.h"
#include "entities/Obstacle.h"
#include "ui/HUD.h"
#include "entities/Coin.h"
#include "data/DataManager.h"
#include "data/PlayerData.h"

#include <vector>
#include <string>

enum GameScreen {

    LOGIN,
    MENU,
    JUGANDO,
    GAMEOVER
};

class Game {
private:

    const int screenWidth = 800;
    const int screenHeight = 450;

    std::vector<Obstacle> obstacles;
    std::vector<Coin> coins;

    float globalSpeed;
    float speedIncrement;

    // OWNER ARCHITECTURE
    Player* player;

    HUD hud;

    DataManager dataManager;
    PlayerData playerData;

    GameScreen currentScreen;

    int creditos;
    int score;
    int highScore;
    int coinsCollectedThisRun;

    float scoreTimer;

    std::string playerName;

    void toggleFullscreen();
    void updateGame();
    void drawGame();
    void drawScaledGame(RenderTexture2D& target);
    void checkCollisions();

public:

    Game();

    void run();

    void resetGame();
};