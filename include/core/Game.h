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
    CARGA_INICIAL,
    INICIO,
    LOGIN,
    MENU,
    RANKING,
    CARGANDO,
    JUGANDO,
    PAUSA,
    CONFIRMAR_SALIDA,
    IMPACTO,
    GAMEOVER
};

enum class LoadingAction {
    NONE,
    LOGIN,
    START_GAME,
    RANKING,
    RETURN_MENU
};

struct LoadingResult {
    bool ok = false;
    std::string mensaje;
    UsuarioApi usuario;
    PartidaApi partida;
    std::vector<RankingItem> ranking;
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
    Texture2D impactTextures[4] {};
    Texture2D menuPlayerRun[3] {};
    Texture2D menuPlayerJump {};
    Texture2D menuPlayerDoubleJump {};
    Texture2D menuDroneTexture {};
    Texture2D menuObstacleTexture {};
    Texture2D menuCoinTexture[2] {};
    Texture2D menuNitroTexture {};
    Texture2D menuShieldTexture {};

    float foregroundOffset;
    float foregroundWidth;
    float menuPreviewTimer = 0.0f;

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

    bool hasShield;
    float shieldTimer;
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
    bool modoPruebaSinApi = false;

    int nivelActual = 1;
    int ultimoScoreReportado = 0;

    std::string mensajeApi;
    std::vector<RankingItem> rankingActual;

    std::chrono::steady_clock::time_point inicioPartida;

    std::vector<std::future<std::string>> finalizacionesPartidaPendientes;
    std::future<LoadingResult> cargaPendiente;

    LoadingAction accionCarga = LoadingAction::NONE;
    GameScreen pantallaErrorCarga = MENU;
    bool cargaPermiteModoLocal = false;
    std::string tituloCarga;
    std::string detalleCarga;
    std::chrono::steady_clock::time_point inicioCarga;

    // OWNER ARCHITECTURE
    Player *player;

    HUD hud;

    DataManager dataManager;
    PlayerData playerData;

    GameScreen currentScreen;
    float screenTransitionAlpha = 0.0f;
    float initialLoadTimer = 0.0f;

    int creditos;
    int score;
    int highScore;
    int gameCost;
    int coinsCollectedThisRun;

    float scoreTimer;
    float scorePerSecond;
    int nitroSpawnCountdown;
    float impactTimer;
    Vector2 impactPosition {};

    std::string playerName;
    std::string loginUsername;
    std::string loginPassword;
    bool loginPasswordActivo = false;

    void toggleFullscreen();

    void updateGame();

    void drawGame();
    void drawLoadingScreen();
    void drawImpactAnimation();

    void drawBackground(); //fondo
    void drawForeground(); //foreground
    void drawScaledGame(RenderTexture2D &target);

    void checkCollisions();

    bool iniciarPartidaApi();

    void reportarScoreApiSiCorresponde();

    void finalizarPartidaApi(const std::string &resultado);

    void finalizarPartidaApiAsync(const std::string &resultado);

    void limpiarFinalizacionesPartidaTerminadas();

    void iniciarCargaLogin();

    void iniciarCargaPartida(GameScreen pantallaError, bool permitirModoLocal);

    void iniciarCargaRanking();

    void iniciarCargaVolverMenu();

    void actualizarCarga();

    void aplicarResultadoCarga(const LoadingResult &resultado);

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

    void setModoPruebaSinApi(bool activo);

    void run();

    void resetGame();
};
