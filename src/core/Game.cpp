#include <core/Game.h>
#include <algorithm>
#include "../../api/GameApiConfig.h"
// Paleta de Colores Neón
const Color NEO_CYAN = {0, 255, 255, 255};
const Color NEO_MAGENTA = {255, 0, 255, 255};
const Color NEO_YELLOW = {253, 249, 0, 255};
const Color NEO_RED = {230, 41, 55, 255};

Game::Game(ApiClient &apiClient)
    : api(apiClient) {
    mensajeApi = "API: esperando accion.";

    player = nullptr;

    //fondo
    transitionAlpha = 0.0f;
    bgOffset = 0.0f;
    bgWidth = 0.0f;

    globalSpeed = 350.0f;
    speedIncrement = 30.0f;

    gameCost = GameApiConfig::COSTO_PARTIDA;

    maxNormalSpeed = 850.0f;
    maxNitroSpeed = 1100.0f;

    hasShield = false;
    nitroActive = false;
    nitroTimer = 0.0f;
    shouldCloseGame = false;

    currentScreen = LOGIN;

    playerData = dataManager.loadPlayerData();

    creditos = playerData.credits;
    highScore = playerData.highScore;
    playerName = playerData.username;

    score = 0;
    scoreTimer = 0.0f;
    coinsCollectedThisRun = 0;
}

void Game::setUsuario(
    const UsuarioApi &usuario
) {
    usuarioActual = usuario;

    sesionIniciada = true;
}

bool Game::iniciarPartidaApi() {
    if (!sesionIniciada || !api.tieneSesion()) {
        mensajeApi = "API: no hay sesion activa.";
        TraceLog(LOG_ERROR, "No hay sesion activa para iniciar partida.");
        return false;
    }

    std::string error;

    bool ok = api.iniciarPartida(
        partidaActual,
        error,
        GameApiConfig::VERSION_JUEGO,
        gameCost
    );

    if (!ok) {
        mensajeApi = "API: no se pudo iniciar partida.";
        TraceLog(LOG_ERROR, TextFormat("No se pudo iniciar partida: %s", error.c_str()));
        return false;
    }

    partidaActiva = true;
    partidaFinalizada = false;

    ultimoScoreReportado = 0;
    nivelActual = 1;

    creditos = partidaActual.saldoDespues;
    playerData.credits = creditos;

    inicioPartida = std::chrono::steady_clock::now();

    TraceLog(
        LOG_INFO,
        TextFormat("Partida iniciada en API. ID: %lld", partidaActual.idPartida)
    );

    mensajeApi = "API: partida iniciada correctamente.";

    return true;
}

void Game::reportarScoreApiSiCorresponde() {
    if (!partidaActiva || partidaFinalizada) {
        return;
    }

    if (score - ultimoScoreReportado < GameApiConfig::REPORTAR_CADA_PUNTOS) {
        return;
    }

    std::string error;

    bool ok = api.reportarScore(
        partidaActual.idPartida,
        score,
        nivelActual,
        error
    );

    if (ok) {
        ultimoScoreReportado = score;
    } else {
        TraceLog(
            LOG_WARNING,
            TextFormat("No se pudo reportar score: %s", error.c_str())
        );
    }
}

int Game::calcularTokensGanados(int scoreFinal) const {
    if (scoreFinal >= GameApiConfig::SCORE_PREMIO_ALTO) {
        return GameApiConfig::PREMIO_ALTO;
    }

    if (scoreFinal >= GameApiConfig::SCORE_PREMIO_BAJO) {
        return GameApiConfig::PREMIO_BAJO;
    }

    return 0;
}

void Game::finalizarPartidaApi(const std::string &resultado) {
    if (!partidaActiva || partidaFinalizada) {
        return;
    }

    auto finPartida = std::chrono::steady_clock::now();

    int duracionSegundos = static_cast<int>(
        std::chrono::duration_cast<std::chrono::seconds>(
            finPartida - inicioPartida
        ).count()
    );

    int tokensGanados = calcularTokensGanados(score);

    std::string error;

    if (score > ultimoScoreReportado) {
        std::string errorScoreFinal;

        bool scoreFinalOk = api.reportarScore(
            partidaActual.idPartida,
            score,
            nivelActual,
            errorScoreFinal
        );

        if (scoreFinalOk) {
            ultimoScoreReportado = score;
        } else {
            TraceLog(
                LOG_WARNING,
                TextFormat("No se pudo reportar score final: %s", errorScoreFinal.c_str())
            );
        }
    }

    bool ok = api.finalizarPartida(
        partidaActual.idPartida,
        score,
        nivelActual,
        resultado,
        duracionSegundos,
        tokensGanados,
        error
    );

    if (ok) {
        mensajeApi = TextFormat("API: partida finalizada. Tokens ganados: %i", tokensGanados);

        TraceLog(
            LOG_INFO,
            TextFormat(
                "Partida finalizada. Resultado: %s | Score: %i | Tokens ganados: %i",
                resultado.c_str(),
                score,
                tokensGanados
            )
        );
    } else {
        mensajeApi = "API: no se pudo finalizar partida.";

        TraceLog(
            LOG_WARNING,
            TextFormat("No se pudo finalizar partida: %s", error.c_str())
        );
    }

    partidaFinalizada = true;
    partidaActiva = false;
}

void Game::consultarRankingApi() {
    if (!sesionIniciada || !api.tieneSesion()) {
        mensajeApi = "API: no hay sesion para ranking.";
        TraceLog(LOG_ERROR, "No hay sesion activa para consultar ranking.");
        return;
    }

    std::vector<RankingItem> ranking;
    std::string error;

    bool ok = api.consultarRanking(
        ranking,
        error
    );

    if (!ok) {
        mensajeApi = "API: no se pudo consultar ranking.";

        TraceLog(
            LOG_ERROR,
            TextFormat("No se pudo consultar ranking: %s", error.c_str())
        );

        return;
    }

    TraceLog(LOG_INFO, "===== RANKING =====");

    if (ranking.empty()) {
        mensajeApi = "API: ranking vacio.";
        TraceLog(LOG_INFO, "Ranking vacio.");
        return;
    }

    mensajeApi = "API: ranking consultado. Revisa consola.";

    for (size_t i = 0; i < ranking.size(); ++i) {
        TraceLog(
            LOG_INFO,
            TextFormat(
                "%i. %s | Score: %i | Nivel: %i",
                static_cast<int>(i + 1),
                ranking[i].username.c_str(),
                ranking[i].bestScore,
                ranking[i].bestNivel
            )
        );
    }
}

void Game::resetGame() {
    if (player != nullptr) {
        delete player;
        player = nullptr;
    }

    player = new Player();

    globalSpeed = 350.0f;

    hasShield = false;
    nitroActive = false;
    nitroTimer = 0.0f;

    if (player != nullptr) {
        player->setNitro(false);
    }

    score = 0;
    scoreTimer = 0.0f;

    coinsCollectedThisRun = 0;

    obstacles.clear();

    obstacles.push_back(
        Obstacle(
            1000,
            310,
            30,
            45,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            1500,
            220,
            40,
            25,
            globalSpeed
        )
    );

    coins.clear();

    coins.push_back(
        Coin(
            900,
            260,
            20,
            globalSpeed
        )
    );

    coins.push_back(
        Coin(
            1250,
            220,
            20,
            globalSpeed
        )
    );

    coins.push_back(
        Coin(
            1600,
            280,
            20,
            globalSpeed
        )
    );
}

void Game::run() {
    InitWindow(
        screenWidth,
        screenHeight,
        "Cyber-Runner"
    );

    InitAudioDevice();

    backgroundMusic = LoadMusicStream("assets/music/fondo.ogg");

    PlayMusicStream(backgroundMusic);

    fondo1 = LoadTexture("assets/textures/fondocyber.png");
    fondo2 = LoadTexture("assets/textures/fondocyber2.png");
    fondo3 = LoadTexture("assets/textures/fondocyber3.png");

    player = new Player();

    SetWindowState(FLAG_WINDOW_RESIZABLE);

    SetTargetFPS(60);

    RenderTexture2D target =
            LoadRenderTexture(
                screenWidth,
                screenHeight
            );

    SetTextureFilter(
        target.texture,
        TEXTURE_FILTER_POINT
    );

    while (!WindowShouldClose() && !shouldCloseGame) {
        //music
        UpdateMusicStream(backgroundMusic);

        toggleFullscreen();

        updateGame();

        BeginTextureMode(target);

        ClearBackground(BLACK);

        drawBackground(); //fondo imp

        drawGame();

        EndTextureMode();

        drawScaledGame(target);
    }

    if (partidaActiva && !partidaFinalizada) {
        finalizarPartidaApi("EXIT");
    }

    UnloadTexture(fondo1);
    UnloadTexture(fondo2);
    UnloadTexture(fondo3);

    UnloadMusicStream(backgroundMusic);

    CloseAudioDevice();

    UnloadRenderTexture(target);

    if (player != nullptr) {
        delete player;
        player = nullptr;
    }

    CloseWindow();
}

//fondo imp
void Game::drawBackground() {
    int etapa = (score / 4000) % 3;

    static int etapaAnterior = 0;

    if (etapa != etapaAnterior) {
        transitionAlpha = 0.0f;
        etapaAnterior = etapa;
    }

    Texture2D fondoActual{};
    Texture2D fondoSiguiente{};

    if (etapa == 0) {
        fondoActual = fondo1;
        fondoSiguiente = fondo2;
    } else if (etapa == 1) {
        fondoActual = fondo2;
        fondoSiguiente = fondo3;
    } else {
        fondoActual = fondo3;
        fondoSiguiente = fondo1;
    }

    float scale =
            static_cast<float>(screenHeight) /
            static_cast<float>(fondoActual.height);

    bgWidth = fondoActual.width * scale;

    Color fadeColor = {
        255,
        255,
        255,
        static_cast<unsigned char>(transitionAlpha * 255)
    };

    DrawTextureEx(
        fondoActual,
        {bgOffset, 0},
        0.0f,
        scale,
        WHITE
    );

    DrawTextureEx(
        fondoActual,
        {bgOffset + bgWidth, 0},
        0.0f,
        scale,
        WHITE
    );

    if (transitionAlpha > 0.0f) {
        DrawTextureEx(
            fondoSiguiente,
            {bgOffset, 0},
            0.0f,
            scale,
            fadeColor
        );

        DrawTextureEx(
            fondoSiguiente,
            {bgOffset + bgWidth, 0},
            0.0f,
            scale,
            fadeColor
        );
    }
}


void Game::toggleFullscreen() {
    if (IsKeyPressed(KEY_F11)) {
        if (!IsWindowFullscreen()) {
            int monitor = GetCurrentMonitor();

            SetWindowSize(
                GetMonitorWidth(monitor),
                GetMonitorHeight(monitor)
            );

            ToggleFullscreen();
        } else {
            ToggleFullscreen();

            SetWindowSize(
                screenWidth,
                screenHeight
            );
        }
    }
}

void Game::updateGame() {
    switch (currentScreen) {
        case LOGIN: {
            bool continuarPressed =
                    IsKeyPressed(KEY_ENTER) ||
                    (
                        IsGamepadAvailable(0) &&
                        (
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)
                        )
                    );

            if (continuarPressed) {
                currentScreen = MENU;
            }

            break;
        }

        case MENU: {
            if (
                IsKeyPressed(KEY_ONE) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
                )
            ) {
                if (iniciarPartidaApi()) {
                    resetGame();

                    currentScreen = JUGANDO;
                } else {
                    mensajeApi = "API: modo prueba local, partida sin servidor.";

                    resetGame();

                    currentScreen = JUGANDO;
                }
            }

            if (
                IsKeyPressed(KEY_THREE) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)
                )
            ) {
                consultarRankingApi();
            }

            if (
                IsKeyPressed(KEY_FOUR) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)
                )
            ) {
                shouldCloseGame = true;
            }

            break;
        }

        case JUGANDO: {
            float deltaTime = GetFrameTime();

            //scrolling imp

            bgOffset -= 25.0f * deltaTime;

            if (bgOffset <= -bgWidth) {
                bgOffset = 0.0f;
            }

            if (transitionAlpha < 1.0f) {
                transitionAlpha += 0.5f * deltaTime;

                if (transitionAlpha > 1.0f) {
                    transitionAlpha = 1.0f;
                }
            }

            scoreTimer += 100.0f * deltaTime;

            score = static_cast<int>(scoreTimer);
            //reportarScoreApiSiCorresponde();

            if (nitroActive) {
                nitroTimer -= deltaTime;

                globalSpeed += 250.0f * deltaTime;

                if (globalSpeed > maxNitroSpeed) {
                    globalSpeed = maxNitroSpeed;
                }

                if (nitroTimer <= 0.0f) {
                    nitroActive = false;

                    if (player != nullptr) {
                        player->setNitro(false);
                    }
                }
            } else {
                globalSpeed += speedIncrement * deltaTime;

                if (globalSpeed > maxNormalSpeed) {
                    globalSpeed = maxNormalSpeed;
                }
            }

            if (player != nullptr) {
                player->update(deltaTime);
            }

            for (auto &obs: obstacles) {
                obs.setSpeed(globalSpeed);

                obs.update(deltaTime);
            }


            for (auto &coin: coins) {
                coin.setSpeed(globalSpeed);

                coin.update(deltaTime);

                if (
                    player != nullptr &&
                    coin.isActive() &&
                    CheckCollisionRecs(
                        player->getRect(),
                        coin.getRect()
                    )
                ) {
                    ItemType itemType = coin.getType();

                    if (itemType == ItemType::CREDIT) {
                        coinsCollectedThisRun++;

                        playerData.totalCoinsCollected++;

                        scoreTimer += 25.0f;
                        score = static_cast<int>(scoreTimer);

                        dataManager.registerCoinCollected(
                            playerData.userId,
                            1
                        );
                    } else if (itemType == ItemType::NITRO) {
                        nitroActive = true;
                        nitroTimer = 3.0f;

                        if (player != nullptr) {
                            player->setNitro(true);
                        }

                        scoreTimer += 50.0f;
                        score = static_cast<int>(scoreTimer);
                    } else if (itemType == ItemType::SHIELD) {
                        hasShield = true;

                        scoreTimer += 50.0f;
                        score = static_cast<int>(scoreTimer);
                    }

                    coin.collect();
                }
            }

            checkCollisions();

            break;
        }

        case GAMEOVER: {
            bool volverMenuPressed =
                    IsKeyPressed(KEY_R) ||
                    (
                        IsGamepadAvailable(0) &&
                        (
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)
                        )
                    );

            if (volverMenuPressed) {
                currentScreen = MENU;
            }

            break;
        }
    }
}

void Game::checkCollisions() {
    if (player == nullptr) {
        return;
    }

    for (auto &obs: obstacles) {
        if (
            CheckCollisionRecs(
                player->getRect(),
                obs.getRect()
            )
        ) {
            if (hasShield) {
                hasShield = false;
                obs.forceRespawn();
                return;
            }

            if (score > highScore) {
                highScore = score;

                playerData.highScore = highScore;
            }

            playerData.credits = creditos;

            dataManager.savePlayerData(playerData);

            dataManager.registerGameFinished(
                playerData.userId,
                score,
                coinsCollectedThisRun
            );

            finalizarPartidaApi("LOSE");

            currentScreen = GAMEOVER;
            return;
        }
    }
}

void Game::drawGame() {
    switch (currentScreen) {
        case LOGIN: {
            DrawText(
                "CYBER-RUNNER",
                250,
                80,
                35,
                NEO_CYAN
            );

            DrawText(
                "[ Presiona ENTER para entrar al sistema ]",
                180,
                350,
                20,
                GRAY
            );

            break;
        }

        case MENU: {
            DrawText(
                "ACCESO CONCEDIDO",
                300,
                40,
                20,
                GREEN
            );

            DrawText(
                "--- TERMINAL DE CONTROL ---",
                220,
                80,
                25,
                NEO_MAGENTA
            );

            DrawRectangleLines(
                250,
                140,
                300,
                40,
                NEO_CYAN
            );

            DrawText(
                "[1] EMPEZAR PARTIDA",
                280,
                150,
                20,
                WHITE
            );

            DrawRectangleLines(
                250,
                200,
                300,
                40,
                WHITE
            );

            DrawText(
                "[2] LOGUEARSE",
                280,
                210,
                20,
                GRAY
            );

            DrawRectangleLines(
                250,
                260,
                300,
                40,
                WHITE
            );

            DrawText(
                "[3] VER RANKING",
                280,
                270,
                20,
                GRAY
            );

            DrawRectangleLines(
                250,
                320,
                300,
                40,
                NEO_RED
            );

            DrawText(
                "[4] SALIR DEL JUEGO",
                280,
                330,
                20,
                WHITE
            );

            DrawText(
                TextFormat("COSTO POR PARTIDA: %i CREDITO(S)", gameCost),
                250,
                380,
                15,
                NEO_YELLOW
            );

            DrawText(
                mensajeApi.c_str(),
                250,
                405,
                15,
                DARKGRAY
            );

            hud.drawMenuHUD(creditos);

            break;
        }

        case JUGANDO: {
            if (player != nullptr) {
                player->draw();
            }

            for (auto &obs: obstacles) {
                obs.draw();
            }

            for (auto &coin: coins) {
                coin.draw();
            }

            DrawLine(
                0,
                350,
                800,
                350,
                NEO_MAGENTA
            );

            hud.drawGameHUD(
                globalSpeed,
                creditos,
                score,
                highScore,
                nitroActive,
                hasShield
            );
            if (hasShield) {
                DrawText("ESCUDO ACTIVO", 600, 45, 15, ORANGE);

                if (player != nullptr) {
                    Vector2 playerCenter = player->getPosition();

                    DrawCircleLines(
                        playerCenter.x,
                        playerCenter.y,
                        35,
                        ORANGE
                    );
                }
            }

            break;
        }

        case GAMEOVER: {
            DrawText(
                "SISTEMA CRITICO: GAME OVER",
                180,
                150,
                30,
                NEO_RED
            );

            DrawText(
                "Presiona [R] para volver al menu",
                250,
                250,
                20,
                WHITE
            );

            break;
        }
    }
}

void Game::drawScaledGame(RenderTexture2D &target) {
    BeginDrawing();

    ClearBackground(BLACK);

    float scale = std::min(
        static_cast<float>(GetScreenWidth()) / screenWidth,
        static_cast<float>(GetScreenHeight()) / screenHeight
    );

    float scaledWidth = screenWidth * scale;
    float scaledHeight = screenHeight * scale;

    float offsetX =
            (GetScreenWidth() - scaledWidth) / 2.0f;

    float offsetY =
            (GetScreenHeight() - scaledHeight) / 2.0f;

    Rectangle source = {
        0.0f,
        0.0f,
        static_cast<float>(target.texture.width),
        -static_cast<float>(target.texture.height)
    };

    Rectangle dest = {
        offsetX,
        offsetY,
        scaledWidth,
        scaledHeight
    };

    DrawTexturePro(
        target.texture,
        source,
        dest,
        {0, 0},
        0.0f,
        WHITE
    );

    EndDrawing();
}
