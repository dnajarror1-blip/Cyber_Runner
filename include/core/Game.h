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
#include <chrono>

enum GameScreen {

    LOGIN,
    MENU,
    JUGANDO,
    GAMEOVER
};

class Game {
private:

    // fondo imp
    float transitionAlpha;
    float bgOffset;
    float bgWidth;
    Texture2D fondo1 {};
    Texture2D fondo2 {};
    Texture2D fondo3 {};

    Music backgroundMusic {};

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

    bool sesionIniciada = false;
    bool partidaActiva = false;
    bool partidaFinalizada = false;

    int nivelActual = 1;
    int ultimoScoreReportado = 0;

    std::string mensajeApi;

    std::chrono::steady_clock::time_point inicioPartida;

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
    void drawBackground(); //fondo
    void drawScaledGame(RenderTexture2D& target);
    void checkCollisions();
    bool iniciarPartidaApi();

    void reportarScoreApiSiCorresponde();
    void finalizarPartidaApi(const std::string& resultado);
    int calcularTokensGanados(int scoreFinal) const;
    void consultarRankingApi();

public:

    explicit Game(ApiClient& apiClient);

    void setUsuario(
     const UsuarioApi& usuario
 );

    void run();

    void resetGame();
};