#pragma once

#include "entities/Player.h"
#include "entities/Obstacle.h"
#include "ui/HUD.h"
#include "entities/Coin.h"
#include "data/DataManager.h"
#include "data/PlayerData.h"
#include "../../api/ApiClient.h"

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

    float maxNormalSpeed;
    float maxNitroSpeed;

    bool hasShield;
    bool nitroActive;
    float nitroTimer;
    bool shouldCloseGame;

    ApiClient& api;

    UsuarioApi usuarioActual;

    PartidaApi partidaActual;

    bool partidaActiva = false;

    bool sesionIniciada = false;

    bool usandoApi = false;

    // OWNER ARCHITECTURE
    Player* player;

    HUD hud;

    DataManager dataManager;
    PlayerData playerData;

    GameScreen currentScreen;

    int creditos;
    int score;
    int highScore;
    int gameCost;
    int coinsCollectedThisRun;

    float scoreTimer;

    std::string playerName;

    void toggleFullscreen();
    void updateGame();
    void drawGame();
    void drawScaledGame(RenderTexture2D& target);
    void checkCollisions();

public:

    explicit Game(ApiClient& apiClient);

    void setUsuario(
     const UsuarioApi& usuario
 );

    void run();

    void resetGame();
};