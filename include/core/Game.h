//
// Created by darwin on 18/04/26.
//

#pragma once
#include "entities/Player.h"
#include "entities/Obstacle.h"
#include <vector>

enum GameScreen { LOGIN, MENU, JUGANDO, GAMEOVER };

class Game {
private:
    std::vector<Obstacle> obstacles;
    int screenWidth = 800;
    int screenHeight = 450;

    GameScreen currentScreen = LOGIN;
    int creditos = 100;

    Player player;

public:
    void run();
    void resetGame();
};



#ifndef CYBER_RUNNER_GAME_H
#define CYBER_RUNNER_GAME_H

#endif //CYBER_RUNNER_GAME_H
