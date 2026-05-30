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

// SECCION: Flujo de pantalla
// Enumera las pantallas principales que puede mostrar el juego.
// Game usa este estado para decidir que actualizar y que dibujar en cada frame.
enum GameScreen {
    CARGA_INICIAL,
    INICIO,
    LOGIN,
    MENU,
    COMO_JUGAR,
    RANKING,
    CARGANDO,
    JUGANDO,
    PAUSA,
    CONFIRMAR_SALIDA,
    IMPACTO,
    GAMEOVER
};

// SECCION: Carga asincrona
// Indica que operacion se esta esperando mientras se muestra la pantalla CARGANDO.
enum class LoadingAction {
    NONE,
    LOGIN,
    START_GAME,
    RANKING,
    RETURN_MENU
};

// Resultado comun para operaciones de carga que pueden consultar la API sin congelar el juego.
struct LoadingResult {
    bool ok = false;
    std::string mensaje;
    UsuarioApi usuario;
    PartidaApi partida;
    std::vector<RankingItem> ranking;
};

// Resultado usado al finalizar una partida en segundo plano.
struct FinalizacionPartidaResult {
    bool ok = false;
    std::string mensaje;
    int tokensGanados = 0;
};

// SECCION: Nucleo del juego
// Coordina menus, gameplay, API, datos locales, audio, entidades y HUD.
class Game {
private:
    // SECCION: Fondo y transiciones visuales
    // fondo imp
    float transitionAlpha;

    float bgOffset;
    float bgWidth;

    Texture2D fondo1{};
    Texture2D fondo2{};
    Texture2D fondo3{};

    // Texturas usadas por menus, previsualizaciones e impactos.
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
    float menuEasterEggTimer = 0.0f;

    // SECCION: Gameplay base
    // hitbox visual del juego
    const float groundY = 350.0f;

    Music backgroundMusic{};

    AudioManager audioManager;

    const int screenWidth = 800;
    const int screenHeight = 450;

    std::vector<Obstacle> obstacles;
    std::vector<Coin> coins;

    // Velocidad global y progresion de dificultad durante una partida.
    float globalSpeed;
    float speedIncrement;

    float maxNormalSpeed;

    bool hasShield;
    float shieldTimer;
    bool nitroActive;
    float nitroTimer;
    bool shouldCloseGame;

    // SECCION: API y sesion
    // Referencias recibidas desde main.cpp; Game no crea estos objetos.
    ApiClient &api;

    LoginManager &loginManager;

    // Datos sincronizados con el backend para el jugador y la partida actual.
    UsuarioApi usuarioActual;

    PartidaApi partidaActual;

    // Banderas que protegen el flujo para no iniciar o finalizar dos veces una partida.
    bool sesionIniciada = false;
    bool partidaActiva = false;
    bool partidaFinalizada = false;
    bool modoPruebaSinApi = false;

    int nivelActual = 1;
    int ultimoScoreReportado = 0;

    std::string mensajeApi;
    std::vector<RankingItem> rankingActual;
    bool mostrarMensajesApi = false;

    // Marca el inicio de la partida para reportar duracion al finalizar.
    std::chrono::steady_clock::time_point inicioPartida;

    // SECCION: Carga asincrona
    // Futures usados para evitar congelamientos mientras se consulta la API.
    std::vector<std::future<FinalizacionPartidaResult>> finalizacionesPartidaPendientes;
    std::future<LoadingResult> cargaPendiente;

    LoadingAction accionCarga = LoadingAction::NONE;
    GameScreen pantallaErrorCarga = MENU;
    bool cargaPermiteModoLocal = false;
    std::string tituloCarga;
    std::string detalleCarga;
    std::chrono::steady_clock::time_point inicioCarga;

    // SECCION: Jugador, HUD y datos locales
    // OWNER ARCHITECTURE
    Player *player;

    HUD hud;

    DataManager dataManager;
    PlayerData playerData;

    GameScreen currentScreen;
    float screenTransitionAlpha = 0.0f;
    float initialLoadTimer = 0.0f;

    // Datos visibles en menu, HUD y resumen de partida.
    int creditos;
    int saldoBasePartida;
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
    bool popupActivo = false;
    std::string popupTitulo;
    std::string popupMensaje;
    std::string popupBoton;

    // FUNCION: Alterna pantalla completa.
    void toggleFullscreen();

    // FUNCION: Actualiza la pantalla activa y el flujo principal.
    void updateGame();

    // FUNCION: Dibuja la pantalla actual.
    void drawGame();
    // FUNCION: Dibuja la pantalla de espera para operaciones asincronas.
    void drawLoadingScreen();
    // FUNCION: Muestra la animacion posterior al choque.
    void drawImpactAnimation();
    // FUNCION: Dibuja popups modales sobre la pantalla actual.
    void drawPopupModal();

    void drawBackground(); //fondo
    void drawForeground(); //foreground
    void drawScaledGame(RenderTexture2D &target);

    void checkCollisions();

    // SECCION: API de partida
    // Inicia una partida en el backend y sincroniza saldo local.
    bool iniciarPartidaApi();

    // FUNCION: Reporta score durante la partida cuando corresponde.
    void reportarScoreApiSiCorresponde();

    // FUNCION: Finaliza una partida de forma directa.
    void finalizarPartidaApi(const std::string &resultado);

    // FUNCION: Finaliza una partida en segundo plano para evitar congelamientos.
    void finalizarPartidaApiAsync(const std::string &resultado);

    // FUNCION: Aplica los resultados pendientes de finalizacion asincrona.
    void limpiarFinalizacionesPartidaTerminadas();

    // SECCION: Cargas de pantallas y API
    void iniciarCargaLogin();

    void iniciarCargaPartida(GameScreen pantallaError, bool permitirModoLocal);

    void iniciarCargaRanking();

    void iniciarCargaVolverMenu();

    void actualizarCarga();

    // FUNCION: Cambia de pantalla y sincroniza datos despues de una carga.
    void aplicarResultadoCarga(const LoadingResult &resultado);

    // SECCION: Popups
    void mostrarPopup(
        const std::string &titulo,
        const std::string &mensaje,
        const std::string &boton
    );

    void cerrarPopup();

    // FUNCION: Convierte monedas recolectadas en tokens ganados.
    int calcularTokensGanados(int scoreFinal) const;

    // SECCION: Ranking y generacion de gameplay
    void consultarRankingApi();

    void generarMonedasEnMatriz(float startX);
    void actualizarGeneracionMonedas();
    void separarObstaculos();

public:
    explicit Game(ApiClient &apiClient, LoginManager &login);

    // FUNCION: Recibe el usuario autenticado y lo refleja en datos locales.
    void setUsuario(
        const UsuarioApi &usuario
    );

    // FUNCION: Activa o desactiva el modo local sin API.
    void setModoPruebaSinApi(bool activo);

    // FUNCION: Inicia la ventana, carga recursos y ejecuta el ciclo principal.
    void run();

    // FUNCION: Reinicia los valores de una nueva partida.
    void resetGame();
};
