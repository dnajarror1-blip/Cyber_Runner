#pragma once

#include "entities/Player.h"
#include "entities/Obstacle.h"
#include "ui/HUD.h"

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
    float globalSpeed;
    float speedIncrement;

    Player player;
    HUD hud;

    GameScreen currentScreen;
    int creditos;
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