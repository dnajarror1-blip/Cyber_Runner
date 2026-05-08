#include <core/Game.h>

#include <algorithm>

// Paleta de Colores Neón
const Color NEO_CYAN = {0, 255, 255, 255};
const Color NEO_MAGENTA = {255, 0, 255, 255};
const Color NEO_YELLOW = {253, 249, 0, 255};
const Color NEO_RED = {230, 41, 55, 255};

Game::Game()
{
    globalSpeed = 350.0f;
    speedIncrement = 30.0f;

    currentScreen = LOGIN;

    player = nullptr;

    playerData = dataManager.loadPlayerData();

    creditos = playerData.credits;
    highScore = playerData.highScore;
    playerName = playerData.username;

    score = 0;
    scoreTimer = 0.0f;
    coinsCollectedThisRun = 0;
}

void Game::resetGame()
{
    if (player != nullptr)
    {
        delete player;
        player = nullptr;
    }

    player = new Player();

    globalSpeed = 350.0f;

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

void Game::run()
{
    InitWindow(
        screenWidth,
        screenHeight,
        "Cyber-Runner"
    );

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

    while (!WindowShouldClose())
    {
        toggleFullscreen();

        updateGame();

        BeginTextureMode(target);

        ClearBackground(BLACK);

        drawGame();

        EndTextureMode();

        drawScaledGame(target);
    }

    UnloadRenderTexture(target);

    if (player != nullptr)
    {
        delete player;
        player = nullptr;
    }

    CloseWindow();
}

void Game::toggleFullscreen()
{
    if (IsKeyPressed(KEY_F11))
    {
        if (!IsWindowFullscreen())
        {
            int monitor = GetCurrentMonitor();

            SetWindowSize(
                GetMonitorWidth(monitor),
                GetMonitorHeight(monitor)
            );

            ToggleFullscreen();
        }
        else
        {
            ToggleFullscreen();

            SetWindowSize(
                screenWidth,
                screenHeight
            );
        }
    }
}

void Game::updateGame()
{
    switch (currentScreen)
    {
        case LOGIN:
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = MENU;
            }

            break;
        }

        case MENU:
        {
            if (IsKeyPressed(KEY_ONE))
            {
                if (creditos >= 5)
                {
                    creditos -= 5;

                    playerData.credits = creditos;
                    playerData.gamesPlayed++;

                    dataManager.savePlayerData(playerData);

                    dataManager.registerGameStarted(
                        playerData.userId
                    );

                    resetGame();

                    currentScreen = JUGANDO;
                }
            }

            if (IsKeyPressed(KEY_FOUR))
            {
                CloseWindow();
            }

            break;
        }

        case JUGANDO:
        {
            float deltaTime = GetFrameTime();

            scoreTimer += 100.0f * deltaTime;

            score = static_cast<int>(scoreTimer);

            globalSpeed += speedIncrement * deltaTime;

            if (globalSpeed > 900.0f)
            {
                globalSpeed = 900.0f;
            }

            if (player != nullptr)
            {
                player->update(deltaTime);
            }

            for (auto& obs : obstacles)
            {
                obs.setSpeed(globalSpeed);

                obs.update(deltaTime);
            }

            checkCollisions();

            for (auto& coin : coins)
            {
                coin.setSpeed(globalSpeed);

                coin.update(deltaTime);

                if (
                    player != nullptr &&
                    coin.isActive() &&
                    CheckCollisionRecs(
                        player->getRect(),
                        coin.getRect()
                    )
                )
                {
                    creditos += 1;

                    coinsCollectedThisRun++;

                    playerData.credits = creditos;

                    playerData.totalCoinsCollected++;

                    score += 25;

                    scoreTimer =
                        static_cast<float>(score);

                    dataManager.savePlayerData(playerData);

                    dataManager.registerCoinCollected(
                        playerData.userId,
                        1
                    );

                    coin.collect();
                }
            }

            break;
        }

        case GAMEOVER:
        {
            if (IsKeyPressed(KEY_R))
            {
                currentScreen = MENU;
            }

            break;
        }
    }
}

void Game::checkCollisions()
{
    if (player == nullptr)
    {
        return;
    }

    for (auto& obs : obstacles)
    {
        if (
            CheckCollisionRecs(
                player->getRect(),
                obs.getRect()
            )
        )
        {
            if (score > highScore)
            {
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

            currentScreen = GAMEOVER;
        }
    }
}

void Game::drawGame()
{
    switch (currentScreen)
    {
        case LOGIN:
        {
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

        case MENU:
        {
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
                "[3] CARGAR USUARIO",
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

            hud.drawMenuHUD(creditos);

            break;
        }

        case JUGANDO:
        {
            if (player != nullptr)
            {
                player->draw();
            }

            for (auto& obs : obstacles)
            {
                obs.draw();
            }

            for (auto& coin : coins)
            {
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
                highScore
            );

            break;
        }

        case GAMEOVER:
        {
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

void Game::drawScaledGame(RenderTexture2D& target)
{
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
