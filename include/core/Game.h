#pragma once

#include "entities/Player.h"
#include "entities/Obstacle.h"
#include "ui/HUD.h"
#include "entities/Coin.h"
#include "data/DataManager.h"
#include "data/PlayerData.h"
#include "audio/AudioManager.h"
#include "../../api/ApiClient.h"
#include "../../auth/LoginManager.h"

#include <vector>
#include <string>
#include <chrono>
#include <future>

enum GameScreen {
    LOGIN,
    MENU,
    RANKING,
    JUGANDO,
    PAUSA,
    CONFIRMAR_SALIDA,
    GAMEOVER
};

class Game {
private:
    // fondo imp
    float transitionAlpha;

    float bgOffset;
    float bgWidth;

    Texture2D fondo1{};
    Texture2D fondo2{};
    Texture2D fondo3{};

    // foregournd
    Texture2D foregroundTexture{};

    float foregroundOffset;
    float foregroundWidth;

    // hitbox visual del juego
    const float groundY = 350.0f;

    Music backgroundMusic{};

    AudioManager audioManager;

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

    ApiClient &api;

    LoginManager &loginManager;

    UsuarioApi usuarioActual;

    PartidaApi partidaActual;

    bool sesionIniciada = false;
    bool partidaActiva = false;
    bool partidaFinalizada = false;

    int nivelActual = 1;
    int ultimoScoreReportado = 0;

    std::string mensajeApi;
    std::vector<RankingItem> rankingActual;

    std::chrono::steady_clock::time_point inicioPartida;

    std::vector<std::future<std::string>> finalizacionesPartidaPendientes;

    // OWNER ARCHITECTURE
    Player *player;

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
    std::string loginUsername;
    std::string loginPassword;
    bool loginPasswordActivo = false;

    void toggleFullscreen();

    void updateGame();

    void drawGame();

    void drawBackground(); //fondo
    void drawForeground(); //foreground
    void drawScaledGame(RenderTexture2D &target);

    void checkCollisions();

    bool iniciarPartidaApi();

    void reportarScoreApiSiCorresponde();

    void finalizarPartidaApi(const std::string &resultado);

    void finalizarPartidaApiAsync(const std::string &resultado);

    void limpiarFinalizacionesPartidaTerminadas();

    int calcularTokensGanados(int scoreFinal) const;

    void consultarRankingApi();

    void generarMonedasEnMatriz(float startX);
    void actualizarGeneracionMonedas();
    void separarObstaculos();

public:
    explicit Game(ApiClient &apiClient, LoginManager &login);

    void setUsuario(
        const UsuarioApi &usuario
    );

    void run();

    void resetGame();
};
