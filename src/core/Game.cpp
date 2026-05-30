#include "audio/AudioManager.h"
#include <core/Game.h>
#include <algorithm>
#include <cmath>
#include <exception>
#include "../../api/GameApiConfig.h"
// Paleta de Colores Neón
const Color NEO_CYAN = {0, 255, 255, 255};
const Color NEO_MAGENTA = {255, 0, 255, 255};
const Color NEO_YELLOW = {253, 249, 0, 255};
const Color NEO_RED = {230, 41, 55, 255};
const Color MENU_PANEL_FILL = {3, 6, 18, 230};
const Color MENU_BUTTON_FILL = {5, 12, 28, 225};
const Color MENU_SHADOW = {0, 0, 0, 210};

static bool gamepadBackPressed()
{
    return IsGamepadAvailable(0) &&
           IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT);
}

static bool shouldAnimateScreenTransition(GameScreen screen)
{
    return screen != JUGANDO && screen != IMPACTO;
}

static void drawCyberText(
    const char *text,
    int x,
    int y,
    int fontSize,
    Color color
)
{
    DrawText(text, x + 2, y + 2, fontSize, MENU_SHADOW);
    DrawText(text, x, y, fontSize, color);
}

static void drawCenteredCyberText(
    const char *text,
    int centerX,
    int y,
    int fontSize,
    Color color
)
{
    int textWidth = MeasureText(text, fontSize);
    drawCyberText(text, centerX - textWidth / 2, y, fontSize, color);
}

static unsigned char blendChannel(
    unsigned char start,
    unsigned char end,
    float amount
)
{
    float value =
            static_cast<float>(start) +
            (static_cast<float>(end) - static_cast<float>(start)) * amount;

    return static_cast<unsigned char>(value);
}

static Color blendColor(
    Color start,
    Color end,
    float amount
)
{
    return {
        blendChannel(start.r, end.r, amount),
        blendChannel(start.g, end.g, amount),
        blendChannel(start.b, end.b, amount),
        blendChannel(start.a, end.a, amount)
    };
}

static Color getAnimatedAccent(Color accent)
{
    float time = static_cast<float>(GetTime());
    float wave = (std::sin(time * 1.7f) + 1.0f) * 0.5f;
    Color paletteColor = blendColor(NEO_CYAN, NEO_MAGENTA, wave);

    float yellowPulse = (std::sin(time * 1.2f + 1.8f) + 1.0f) * 0.5f;
    paletteColor = blendColor(paletteColor, NEO_YELLOW, yellowPulse * 0.35f);

    Color result = blendColor(accent, paletteColor, 0.45f);
    result.a = accent.a;

    return result;
}

static int getServerBestScore(const std::vector<RankingItem> &ranking)
{
    int bestScore = 0;

    for (const RankingItem &item : ranking) {
        bestScore = std::max(bestScore, item.bestScore);
    }

    return bestScore;
}

static float getMenuPreviewX(
    float startX,
    float speed,
    float timer
)
{
    const float loopWidth = 1680.0f;
    float x = std::fmod(startX - timer * speed, loopWidth);

    if (x < -160.0f) {
        x += loopWidth;
    }

    return x;
}

static void drawMenuPreviewTexture(
    Texture2D texture,
    Rectangle dest,
    Color tint,
    Color fallback
)
{
    if (texture.id == 0) {
        DrawRectangleRec(dest, fallback);
        return;
    }

    Rectangle source = {
        0.0f,
        0.0f,
        static_cast<float>(texture.width),
        static_cast<float>(texture.height)
    };

    DrawTexturePro(
        texture,
        source,
        dest,
        {0.0f, 0.0f},
        0.0f,
        tint
    );
}

static void drawCyberPanel(
    int x,
    int y,
    int width,
    int height,
    Color accent
)
{
    Rectangle panel = {
        static_cast<float>(x),
        static_cast<float>(y),
        static_cast<float>(width),
        static_cast<float>(height)
    };

    Color animatedAccent = getAnimatedAccent(accent);
    Color softAccent = animatedAccent;
    softAccent.a = 95;

    DrawRectangleRec(panel, MENU_PANEL_FILL);
    DrawRectangleLinesEx(panel, 2.0f, animatedAccent);
    DrawRectangleLines(
        x + 5,
        y + 5,
        width - 10,
        height - 10,
        softAccent
    );
}

static Vector2 getBorderPoint(Rectangle rect, float progress)
{
    float perimeter = rect.width * 2.0f + rect.height * 2.0f;
    float distance = std::fmod(progress, 1.0f) * perimeter;

    if (distance < rect.width) {
        return {rect.x + distance, rect.y};
    }

    distance -= rect.width;

    if (distance < rect.height) {
        return {rect.x + rect.width, rect.y + distance};
    }

    distance -= rect.height;

    if (distance < rect.width) {
        return {rect.x + rect.width - distance, rect.y + rect.height};
    }

    distance -= rect.width;

    return {rect.x, rect.y + rect.height - distance};
}

static void drawMovingBorderGlow(
    Rectangle rect,
    Color color,
    float phase
)
{
    float time = static_cast<float>(GetTime());
    float progress = std::fmod(time * 0.23f + phase, 1.0f);
    Vector2 glow = getBorderPoint(rect, progress);

    Color soft = color;
    soft.a = 95;

    Color bright = color;
    bright.a = 230;

    DrawCircleGradient(
        static_cast<int>(glow.x),
        static_cast<int>(glow.y),
        18.0f,
        soft,
        {0, 0, 0, 0}
    );
    DrawCircle(
        static_cast<int>(glow.x),
        static_cast<int>(glow.y),
        2.5f,
        bright
    );
}

static void drawRetroRoundButton(
    float x,
    float y,
    const char *label,
    Color accent
)
{
    DrawCircle(
        static_cast<int>(x + 2.0f),
        static_cast<int>(y + 2.0f),
        15.0f,
        {0, 0, 0, 190}
    );
    DrawCircle(
        static_cast<int>(x),
        static_cast<int>(y),
        15.0f,
        MENU_BUTTON_FILL
    );
    DrawCircleLines(
        static_cast<int>(x),
        static_cast<int>(y),
        15.0f,
        accent
    );
    DrawCircleLines(
        static_cast<int>(x),
        static_cast<int>(y),
        11.0f,
        {255, 255, 255, 80}
    );

    int textWidth = MeasureText(label, 14);
    drawCyberText(
        label,
        static_cast<int>(x - textWidth / 2.0f),
        static_cast<int>(y - 8.0f),
        14,
        WHITE
    );
}

static void drawRetroPillButton(
    Rectangle rect,
    const char *label,
    Color accent
)
{
    DrawRectangleRec(
        {rect.x + 2.0f, rect.y + 2.0f, rect.width, rect.height},
        {0, 0, 0, 190}
    );
    DrawRectangleRec(rect, MENU_BUTTON_FILL);
    DrawRectangleLinesEx(rect, 1.5f, accent);
    DrawRectangleLines(
        static_cast<int>(rect.x + 4.0f),
        static_cast<int>(rect.y + 4.0f),
        static_cast<int>(rect.width - 8.0f),
        static_cast<int>(rect.height - 8.0f),
        {255, 255, 255, 70}
    );

    int textWidth = MeasureText(label, 13);
    drawCyberText(
        label,
        static_cast<int>(rect.x + (rect.width - textWidth) / 2.0f),
        static_cast<int>(rect.y + 6.0f),
        13,
        WHITE
    );
}

static void drawRetroActionButton(
    Rectangle rect,
    const char *buttonLabel,
    const char *text,
    Color accent
)
{
    DrawRectangleRec(rect, MENU_BUTTON_FILL);
    DrawRectangleRec(
        {rect.x, rect.y, 5.0f, rect.height},
        accent
    );
    DrawRectangleLinesEx(rect, 1.5f, accent);

    drawRetroRoundButton(
        rect.x + 30.0f,
        rect.y + rect.height / 2.0f,
        buttonLabel,
        accent
    );

    drawCyberText(
        text,
        static_cast<int>(rect.x + 62.0f),
        static_cast<int>(rect.y + (rect.height - 18.0f) / 2.0f),
        18,
        WHITE
    );
}

Game::Game(ApiClient &apiClient, LoginManager &login)
    : api(apiClient),
      loginManager(login) {
    mensajeApi = "API: esperando accion.";

    player = nullptr;

    //fondo
    transitionAlpha = 0.0f;
    bgOffset = 0.0f;
    bgWidth = 0.0f;

    foregroundOffset = 0.0f;
    foregroundWidth = 0.0f;

    globalSpeed = 350.0f;
    speedIncrement = 14.0f;

    gameCost = GameApiConfig::COSTO_PARTIDA;

    maxNormalSpeed = 1600.0f;

    hasShield = false;
    shieldTimer = 0.0f;
    nitroActive = false;
    nitroTimer = 0.0f;
    shouldCloseGame = false;

    currentScreen = CARGA_INICIAL;
    screenTransitionAlpha = 0.0f;
    initialLoadTimer = 0.0f;

    playerData = dataManager.loadPlayerData();

    creditos = playerData.credits;
    saldoBasePartida = creditos;
    highScore = playerData.highScore;
    playerName = playerData.username;

    score = 0;
    scoreTimer = 0.0f;
    scorePerSecond = 55.0f;
    nitroSpawnCountdown = 0;
    coinsCollectedThisRun = 0;
    menuPreviewTimer = 0.0f;
    menuEasterEggTimer = 0.0f;
    impactTimer = 0.0f;
    impactPosition = {0.0f, 0.0f};
}

void Game::setUsuario(
    const UsuarioApi &usuario
) {
    usuarioActual = usuario;

    sesionIniciada = true;

    playerName = usuarioActual.username;
    creditos = usuarioActual.saldoTokens;
    saldoBasePartida = creditos;
    playerData.username = playerName;
    playerData.credits = creditos;
    dataManager.savePlayerData(playerData);
}

void Game::setModoPruebaSinApi(bool activo)
{
    modoPruebaSinApi = activo;
}

bool Game::iniciarPartidaApi() {
    if (!sesionIniciada || !api.tieneSesion()) {
        mensajeApi = "API: no hay sesion activa.";
        TraceLog(LOG_ERROR, "No hay sesion activa para iniciar partida.");
        return false;
    }

    int saldoAntesLocal = creditos;

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

    int saldoDespuesCalculado = partidaActual.saldoDespues;

    if (
        partidaActual.saldoAntes == 0 &&
        partidaActual.saldoDespues == 0 &&
        saldoAntesLocal > 0
    )
    {
        saldoDespuesCalculado = saldoAntesLocal - gameCost;

        if (saldoDespuesCalculado < 0)
        {
            saldoDespuesCalculado = 0;
        }
    }

    creditos = saldoDespuesCalculado;
    saldoBasePartida = creditos;
    usuarioActual.saldoTokens = creditos;

    playerData.credits = creditos;
    dataManager.savePlayerData(playerData);

    TraceLog(
        LOG_INFO,
        TextFormat(
            "Creditos al iniciar partida. Local antes: %i | API antes: %i | API despues: %i | Usado: %i",
            saldoAntesLocal,
            partidaActual.saldoAntes,
            partidaActual.saldoDespues,
            creditos
        )
    );

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
    (void) scoreFinal;

    return coinsCollectedThisRun / 5;
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

void Game::finalizarPartidaApiAsync(const std::string &resultado) {
    if (!partidaActiva || partidaFinalizada) {
        return;
    }

    auto finPartida = std::chrono::steady_clock::now();

    int duracionSegundos = static_cast<int>(
        std::chrono::duration_cast<std::chrono::seconds>(
            finPartida - inicioPartida
        ).count()
    );

    long long idPartida = partidaActual.idPartida;
    int scoreFinal = score;
    int nivelFinal = nivelActual;
    int ultimoScoreFinalReportado = ultimoScoreReportado;
    int tokensGanados = calcularTokensGanados(scoreFinal);
    std::string resultadoFinal = resultado;

    partidaFinalizada = true;
    partidaActiva = false;
    mensajeApi = "API: finalizando partida...";

    finalizacionesPartidaPendientes.push_back(
        std::async(
            std::launch::async,
            [this,
 idPartida,
 scoreFinal,
 nivelFinal,
 ultimoScoreFinalReportado,
 duracionSegundos,
 tokensGanados,
 resultadoFinal]() -> FinalizacionPartidaResult {

                FinalizacionPartidaResult resultadoApi;
                try {
                    if (scoreFinal > ultimoScoreFinalReportado) {
                        std::string errorScoreFinal;

                        api.reportarScore(
                            idPartida,
                            scoreFinal,
                            nivelFinal,
                            errorScoreFinal
                        );
                    }

                    std::string error;

                    bool ok = api.finalizarPartida(
                        idPartida,
                        scoreFinal,
                        nivelFinal,
                        resultadoFinal,
                        duracionSegundos,
                        tokensGanados,
                        error
                    );

                    if (ok) {
    resultadoApi.ok = true;
    resultadoApi.tokensGanados = tokensGanados;
    resultadoApi.mensaje =
        "API: partida finalizada. Tokens ganados: " +
        std::to_string(tokensGanados);

    return resultadoApi;
}

resultadoApi.ok = false;
resultadoApi.tokensGanados = 0;
resultadoApi.mensaje = "API: no se pudo finalizar partida.";

return resultadoApi;

                } catch (const std::exception &e) {
     resultadoApi.ok = false;
     resultadoApi.tokensGanados = 0;
     resultadoApi.mensaje =
         std::string("API: error finalizando partida: ") + e.what();

     return resultadoApi;
 } catch (...) {
     resultadoApi.ok = false;
     resultadoApi.tokensGanados = 0;
     resultadoApi.mensaje = "API: error desconocido finalizando partida.";

     return resultadoApi;
 }
            }
        )
    );
}

void Game::limpiarFinalizacionesPartidaTerminadas() {
    auto it = finalizacionesPartidaPendientes.begin();

    while (it != finalizacionesPartidaPendientes.end()) {
        if (it->wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            FinalizacionPartidaResult resultado = it->get();

            mensajeApi = resultado.mensaje;

            if (resultado.ok) {
                int saldoBaseAntesFinalizar = saldoBasePartida;

                creditos = saldoBaseAntesFinalizar + resultado.tokensGanados;
                usuarioActual.saldoTokens = creditos;
                saldoBasePartida = creditos;

                playerData.credits = creditos;
                dataManager.savePlayerData(playerData);

                TraceLog(
                    LOG_INFO,
                    TextFormat(
                        "Creditos sincronizados. Saldo base: %i | Tokens ganados: %i | Creditos actuales: %i",
                        saldoBaseAntesFinalizar,
                        resultado.tokensGanados,
                        creditos
                    )
                );
            }

            it = finalizacionesPartidaPendientes.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::mostrarPopup(
    const std::string &titulo,
    const std::string &mensaje,
    const std::string &boton
) {
    popupTitulo = titulo;
    popupMensaje = mensaje;
    popupBoton = boton;
    popupActivo = true;
}

void Game::cerrarPopup() {
    popupActivo = false;
    popupTitulo.clear();
    popupMensaje.clear();
    popupBoton.clear();
}

void Game::iniciarCargaLogin() {
    if (cargaPendiente.valid()) {
        return;
    }

    std::string username = loginUsername;
    std::string password = loginPassword;

    accionCarga = LoadingAction::LOGIN;
    pantallaErrorCarga = LOGIN;
    cargaPermiteModoLocal = false;
    tituloCarga = "INICIANDO SESION";
    detalleCarga = "Validando usuario con el servidor";
    inicioCarga = std::chrono::steady_clock::now();
    currentScreen = CARGANDO;

    cargaPendiente = std::async(
        std::launch::async,
        [this, username, password]() -> LoadingResult {
            LoadingResult resultado;
            std::string error;

            resultado.ok = loginManager.iniciarSesion(
                username,
                password,
                error
            );

            if (!resultado.ok) {
                resultado.mensaje = "LOGIN: " + error;
                return resultado;
            }

            resultado.usuario = loginManager.getUsuarioActual();
            resultado.mensaje = "API: login correcto.";

            return resultado;
        }
    );
}

void Game::iniciarCargaPartida(GameScreen pantallaError, bool permitirModoLocal) {
    if (cargaPendiente.valid()) {
        return;
    }

    if (!sesionIniciada) {
        mensajeApi = "Debes iniciar sesion antes de jugar.";
        return;
    }

    if (!api.tieneSesion() && !modoPruebaSinApi) {
        mensajeApi = "Debes iniciar sesion antes de jugar.";
        return;
    }

    accionCarga = LoadingAction::START_GAME;
    pantallaErrorCarga = pantallaError;
    cargaPermiteModoLocal = permitirModoLocal;
    tituloCarga = "PREPARANDO PARTIDA";
    detalleCarga = "Reservando partida y sincronizando tokens";
    inicioCarga = std::chrono::steady_clock::now();
    currentScreen = CARGANDO;

    int costoPartida = gameCost;

    cargaPendiente = std::async(
        std::launch::async,
        [this, costoPartida]() -> LoadingResult {
            LoadingResult resultado;

            if (!sesionIniciada || (!api.tieneSesion() && !modoPruebaSinApi)) {
                resultado.mensaje = "API: no hay sesion activa.";
                return resultado;
            }

            if (modoPruebaSinApi && !api.tieneSesion()) {
                resultado.mensaje = "API: modo prueba local, partida sin servidor.";
                return resultado;
            }

            std::string error;
            PartidaApi partida;

            resultado.ok = api.iniciarPartida(
                partida,
                error,
                GameApiConfig::VERSION_JUEGO,
                costoPartida
            );

            if (!resultado.ok) {
                resultado.mensaje = "API: no se pudo iniciar partida.";
                TraceLog(LOG_ERROR, TextFormat("No se pudo iniciar partida: %s", error.c_str()));
                return resultado;
            }

            resultado.partida = partida;
            resultado.mensaje = "API: partida iniciada correctamente.";

            std::string rankingError;
            std::vector<RankingItem> ranking;

            if (api.consultarRanking(ranking, rankingError)) {
                resultado.ranking = ranking;
            } else {
                TraceLog(
                    LOG_WARNING,
                    TextFormat("No se pudo sincronizar record del servidor: %s", rankingError.c_str())
                );
            }

            return resultado;
        }
    );
}

void Game::iniciarCargaRanking() {
    if (cargaPendiente.valid()) {
        return;
    }

    rankingActual.clear();

    if (!sesionIniciada || !api.tieneSesion()) {
        mensajeApi = "API: no hay sesion para ranking.";
        TraceLog(LOG_ERROR, "No hay sesion activa para consultar ranking.");
        currentScreen = RANKING;
        return;
    }

    accionCarga = LoadingAction::RANKING;
    pantallaErrorCarga = RANKING;
    cargaPermiteModoLocal = false;
    tituloCarga = "CARGANDO RANKING";
    detalleCarga = "Consultando mejores corredores";
    inicioCarga = std::chrono::steady_clock::now();
    currentScreen = CARGANDO;

    cargaPendiente = std::async(
        std::launch::async,
        [this]() -> LoadingResult {
            LoadingResult resultado;
            std::string error;
            std::vector<RankingItem> ranking;

            resultado.ok = api.consultarRanking(
                ranking,
                error
            );

            if (!resultado.ok) {
                resultado.mensaje = "API: no se pudo consultar ranking.";
                TraceLog(LOG_ERROR, TextFormat("No se pudo consultar ranking: %s", error.c_str()));
                return resultado;
            }

            resultado.ranking = ranking;
            resultado.mensaje = ranking.empty()
                ? "API: ranking vacio."
                : "API: ranking consultado.";

            return resultado;
        }
    );
}

void Game::iniciarCargaVolverMenu() {
    if (cargaPendiente.valid()) {
        return;
    }

    accionCarga = LoadingAction::RETURN_MENU;
    pantallaErrorCarga = MENU;
    cargaPermiteModoLocal = false;
    tituloCarga = "REGRESANDO AL MENU";
    detalleCarga = "Cerrando la partida actual";
    inicioCarga = std::chrono::steady_clock::now();
    currentScreen = CARGANDO;

    cargaPendiente = std::async(
        std::launch::async,
        []() -> LoadingResult {
            LoadingResult resultado;
            resultado.ok = true;
            resultado.mensaje = "API: regresando al menu.";
            return resultado;
        }
    );
}

void Game::actualizarCarga() {
    if (!cargaPendiente.valid()) {
        currentScreen = pantallaErrorCarga;
        accionCarga = LoadingAction::NONE;
        return;
    }

    if (cargaPendiente.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        return;
    }

    auto ahora = std::chrono::steady_clock::now();
    auto duracionMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        ahora - inicioCarga
    ).count();

    if (duracionMs < 350) {
        return;
    }

    LoadingResult resultado = cargaPendiente.get();
    aplicarResultadoCarga(resultado);
}

void Game::aplicarResultadoCarga(const LoadingResult &resultado) {
    switch (accionCarga) {
        case LoadingAction::LOGIN: {
            if (!resultado.ok) {
                mensajeApi = resultado.mensaje;
                currentScreen = LOGIN;
                mostrarPopup(
                    "LOGIN RECHAZADO",
                    "Usuario o contrasena incorrectos. Verifica tus datos.",
                    "ACEPTAR"
                );
                break;
            }

            setUsuario(resultado.usuario);
            mensajeApi = resultado.mensaje;
            currentScreen = MENU;
            break;
        }

        case LoadingAction::START_GAME: {
            if (!resultado.ok) {
                if (cargaPermiteModoLocal) {
                    mensajeApi = "API: modo prueba local, partida sin servidor.";
                    resetGame();
                    currentScreen = JUGANDO;
                } else {
                    mensajeApi = resultado.mensaje;
                    currentScreen = pantallaErrorCarga;
                }

                break;
            }

                int saldoAntesLocal = creditos;

            partidaActual = resultado.partida;
            partidaActiva = true;
            partidaFinalizada = false;
            ultimoScoreReportado = 0;
            nivelActual = 1;

                int saldoDespuesCalculado = partidaActual.saldoDespues;

                if (
                    partidaActual.saldoAntes == 0 &&
                    partidaActual.saldoDespues == 0 &&
                    saldoAntesLocal > 0
                )
                {
                    saldoDespuesCalculado = saldoAntesLocal - gameCost;

                    if (saldoDespuesCalculado < 0)
                    {
                        saldoDespuesCalculado = 0;
                    }
                }

                creditos = saldoDespuesCalculado;
                saldoBasePartida = creditos;
                usuarioActual.saldoTokens = creditos;

                playerData.credits = creditos;
                dataManager.savePlayerData(playerData);

                TraceLog(
                    LOG_INFO,
                    TextFormat(
                        "Creditos al iniciar partida. Local antes: %i | API antes: %i | API despues: %i | Usado: %i",
                        saldoAntesLocal,
                        partidaActual.saldoAntes,
                        partidaActual.saldoDespues,
                        creditos
                    )
                );

            if (!resultado.ranking.empty()) {
                rankingActual = resultado.ranking;
                highScore = getServerBestScore(rankingActual);
            }

            inicioPartida = std::chrono::steady_clock::now();
            mensajeApi = resultado.mensaje;

            TraceLog(
                LOG_INFO,
                TextFormat("Partida iniciada en API. ID: %lld", partidaActual.idPartida)
            );

            resetGame();
            currentScreen = JUGANDO;
            break;
        }

        case LoadingAction::RANKING: {
            rankingActual = resultado.ranking;
            if (!rankingActual.empty()) {
                highScore = getServerBestScore(rankingActual);
            }

            mensajeApi = resultado.mensaje;
            currentScreen = RANKING;
            break;
        }

        case LoadingAction::RETURN_MENU: {
            mensajeApi = resultado.mensaje;
            currentScreen = MENU;
            break;
        }

        case LoadingAction::NONE: {
            currentScreen = pantallaErrorCarga;
            break;
        }
    }

    accionCarga = LoadingAction::NONE;
    cargaPermiteModoLocal = false;
}

void Game::consultarRankingApi() {
    rankingActual.clear();

    if (!sesionIniciada || !api.tieneSesion()) {
        mensajeApi = "API: no hay sesion para ranking.";
        TraceLog(LOG_ERROR, "No hay sesion activa para consultar ranking.");
        currentScreen = RANKING;
        return;
    }

    std::string error;

    bool ok = api.consultarRanking(
        rankingActual,
        error
    );

    if (!ok) {
        mensajeApi = "API: no se pudo consultar ranking.";
        currentScreen = RANKING;

        TraceLog(
            LOG_ERROR,
            TextFormat("No se pudo consultar ranking: %s", error.c_str())
        );

        return;
    }

    TraceLog(LOG_INFO, "===== RANKING =====");

    if (rankingActual.empty()) {
        mensajeApi = "API: ranking vacio.";
        TraceLog(LOG_INFO, "Ranking vacio.");
        currentScreen = RANKING;
        return;
    }

    highScore = getServerBestScore(rankingActual);

    mensajeApi = "API: ranking consultado.";
    currentScreen = RANKING;

    for (size_t i = 0; i < rankingActual.size(); ++i) {
        TraceLog(
            LOG_INFO,
            TextFormat(
                "%i. %s | Score: %i | Nivel: %i",
                static_cast<int>(i + 1),
                rankingActual[i].username.c_str(),
                rankingActual[i].bestScore,
                rankingActual[i].bestNivel
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
    shieldTimer = 0.0f;
    nitroActive = false;
    nitroTimer = 0.0f;

    if (player != nullptr) {
        player->setNitro(false);
    }

    score = 0;
    scoreTimer = 0.0f;

    coinsCollectedThisRun = 0;
    nitroSpawnCountdown = GetRandomValue(2, 4);
    impactTimer = 0.0f;
    impactPosition = {0.0f, 0.0f};

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

    obstacles.push_back(
        Obstacle(
            2050,
            310,
            30,
            45,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            2600,
            220,
            40,
            25,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            3150,
            310,
            30,
            45,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            3700,
            220,
            40,
            25,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            4250,
            310,
            30,
            45,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            4800,
            220,
            40,
            25,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            5350,
            310,
            30,
            45,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            5900,
            220,
            40,
            25,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            6450,
            310,
            30,
            45,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            7000,
            220,
            40,
            25,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            7550,
            310,
            30,
            45,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            8100,
            220,
            40,
            25,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            8650,
            310,
            30,
            45,
            globalSpeed
        )
    );

    obstacles.push_back(
        Obstacle(
            9200,
            205,
            40,
            25,
            globalSpeed
        )
    );

    coins.clear();

    for (int i = 0; i < 14; ++i) {
        coins.push_back(
            Coin(
                900 + i * 32,
                260,
                20,
                globalSpeed
            )
        );
    }

    generarMonedasEnMatriz(900.0f);
}

void Game::generarMonedasEnMatriz(float startX) {
    if (coins.empty()) {
        return;
    }

    int patron = GetRandomValue(0, 2);
    bool spawnNitro = nitroSpawnCountdown <= 0;
    bool monedasCercaDron = false;
    Rectangle dronCercano {};

    if (spawnNitro) {
        nitroSpawnCountdown = GetRandomValue(4, 6);
    } else {
        nitroSpawnCountdown--;
    }

    for (auto &obs: obstacles) {
        Rectangle rect = obs.getRect();

        if (
            obs.getType() == ObstacleType::AIR &&
            rect.x > startX - 120.0f &&
            rect.x < startX + 760.0f
        ) {
            dronCercano = rect;
            monedasCercaDron = true;
            break;
        }
    }

    for (size_t i = 0; i < coins.size(); ++i) {
        size_t shieldIndex = coins.size() - 2;
        size_t nitroIndex = coins.size() - 1;

        if (i == shieldIndex) {
            coins[i].reset(
                startX + 520.0f,
                static_cast<float>(GetRandomValue(205, 295)),
                ItemType::SHIELD
            );

            continue;
        }

        if (i == nitroIndex) {
            coins[i].reset(
                startX + 740.0f,
                static_cast<float>(GetRandomValue(205, 295)),
                spawnNitro ? ItemType::NITRO : ItemType::CREDIT
            );

            continue;
        }

        if (monedasCercaDron && (i == 10 || i == 11)) {
            float offsetX = i == 10 ? -34.0f : 34.0f;
            float offsetY = i == 10 ? -18.0f : 20.0f;

            coins[i].reset(
                dronCercano.x + offsetX,
                dronCercano.y + offsetY,
                ItemType::CREDIT
            );

            continue;
        }

        int columna = static_cast<int>(i % 4);
        int fila = static_cast<int>(i / 4);

        float x = startX + columna * 34.0f;
        float y = 0.0f;

        if (patron == 0) {
            y = 225.0f + fila * 34.0f;
        } else if (patron == 1) {
            y = 250.0f + fila * 34.0f;
        } else {
            y = 215.0f + static_cast<float>(columna % 2) * 34.0f + fila * 26.0f;
        }

        coins[i].reset(x, y, ItemType::CREDIT);
    }
}

void Game::actualizarGeneracionMonedas() {
    for (auto &coin: coins) {
        if (coin.isActive()) {
            return;
        }
    }

    float startX = 850.0f + static_cast<float>(GetRandomValue(180, 360));

    for (auto &obs: obstacles) {
        Rectangle rect = obs.getRect();

        if (rect.x > 900.0f) {
            startX = rect.x - static_cast<float>(GetRandomValue(135, 180));
            break;
        }
    }

    if (startX < 830.0f) {
        startX = 830.0f;
    }

    generarMonedasEnMatriz(startX);
}

void Game::separarObstaculos() {
    const float minSpacing = 390.0f;

    for (size_t i = 0; i < obstacles.size(); ++i) {
        for (size_t j = i + 1; j < obstacles.size(); ++j) {
            Rectangle a = obstacles[i].getRect();
            Rectangle b = obstacles[j].getRect();

            float distancia = a.x > b.x ? a.x - b.x : b.x - a.x;

            if (distancia >= minSpacing) {
                continue;
            }

            size_t mover = a.x < b.x ? j : i;
            float baseX = std::max(a.x, b.x);
            Rectangle moverRect = obstacles[mover].getRect();

            if (moverRect.x < static_cast<float>(screenWidth) + 120.0f) {
                continue;
            }

            obstacles[mover].respawn(
                baseX + minSpacing + static_cast<float>(GetRandomValue(80, 220))
            );
        }
    }
}

void Game::run() {
    InitWindow(
        screenWidth,
        screenHeight,
        "Cyber-Runner"
    );
    SetExitKey(KEY_NULL);

    InitAudioDevice();

    audioManager.load();

    backgroundMusic = LoadMusicStream("assets/music/fondo.ogg");

    PlayMusicStream(backgroundMusic);

    fondo1 = LoadTexture("assets/textures/fondocyber.png");
    fondo2 = LoadTexture("assets/textures/fondocyber2.png");
    fondo3 = LoadTexture("assets/textures/fondocyber3.png");
    foregroundTexture = LoadTexture("assets/textures/foreground.png");
    impactTextures[0] = LoadTexture("assets/explotion1.png");
    impactTextures[1] = LoadTexture("assets/explotion2.png");
    impactTextures[2] = LoadTexture("assets/explotion3.png");
    impactTextures[3] = LoadTexture("assets/explotion4.png");
    menuPlayerRun[0] = LoadTexture("assets/player1.png");
    menuPlayerRun[1] = LoadTexture("assets/player2.png");
    menuPlayerRun[2] = LoadTexture("assets/player3.png");
    menuPlayerJump = LoadTexture("assets/player4.png");
    menuPlayerDoubleJump = LoadTexture("assets/playersal1.png");
    menuDroneTexture = LoadTexture("assets/dron.png");
    menuObstacleTexture = LoadTexture("assets/groundobstacle.png");
    menuCoinTexture[0] = LoadTexture("assets/coin1.png");
    menuCoinTexture[1] = LoadTexture("assets/coin2.png");
    menuNitroTexture = LoadTexture("assets/nitro.png");
    menuShieldTexture = LoadTexture("assets/escudo.png");
    hud.loadAssets();

    SetTextureFilter(
    foregroundTexture,
    TEXTURE_FILTER_POINT
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

    while (!WindowShouldClose() && !shouldCloseGame) {
        //music
        UpdateMusicStream(backgroundMusic);

        audioManager.update();

        toggleFullscreen();

        updateGame();

        BeginTextureMode(target);

        ClearBackground(BLACK);

        drawBackground(); //fondo imp

        drawForeground(); // foregorung

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

    UnloadTexture(foregroundTexture);

    for (auto &texture: impactTextures) {
        if (texture.id) {
            UnloadTexture(texture);
            texture = {};
        }
    }

    for (auto &texture: menuPlayerRun) {
        if (texture.id) {
            UnloadTexture(texture);
            texture = {};
        }
    }

    if (menuPlayerJump.id) {
        UnloadTexture(menuPlayerJump);
        menuPlayerJump = {};
    }

    if (menuPlayerDoubleJump.id) {
        UnloadTexture(menuPlayerDoubleJump);
        menuPlayerDoubleJump = {};
    }

    if (menuDroneTexture.id) {
        UnloadTexture(menuDroneTexture);
        menuDroneTexture = {};
    }

    if (menuObstacleTexture.id) {
        UnloadTexture(menuObstacleTexture);
        menuObstacleTexture = {};
    }

    for (auto &texture: menuCoinTexture) {
        if (texture.id) {
            UnloadTexture(texture);
            texture = {};
        }
    }

    if (menuNitroTexture.id) {
        UnloadTexture(menuNitroTexture);
        menuNitroTexture = {};
    }

    if (menuShieldTexture.id) {
        UnloadTexture(menuShieldTexture);
        menuShieldTexture = {};
    }

    hud.unloadAssets();

    UnloadMusicStream(backgroundMusic);

    audioManager.unload();

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

void Game::drawForeground()
{
    if (foregroundTexture.id == 0)
    {
        return;
    }

    const float foregroundHeight = 100.0f;

    const float scale =
        foregroundHeight /
        static_cast<float>(foregroundTexture.height);

    foregroundWidth =
        static_cast<float>(foregroundTexture.width) * scale;

    Rectangle source = {
        0.0f,
        0.0f,
        static_cast<float>(foregroundTexture.width),
        static_cast<float>(foregroundTexture.height)
    };

    Rectangle dest1 = {
        foregroundOffset,
        groundY,
        foregroundWidth,
        foregroundHeight
    };

    Rectangle dest2 = {
        foregroundOffset + foregroundWidth,
        groundY,
        foregroundWidth,
        foregroundHeight
    };

    DrawTexturePro(
        foregroundTexture,
        source,
        dest1,
        {0,0},
        0.0f,
        WHITE
    );

    DrawTexturePro(
        foregroundTexture,
        source,
        dest2,
        {0,0},
        0.0f,
        WHITE
    );
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
    GameScreen screenBeforeUpdate = currentScreen;
    float frameTime = GetFrameTime();

    if (popupActivo) {
        bool cerrarPopupPressed =
                IsKeyPressed(KEY_ENTER) ||
                IsKeyPressed(KEY_SPACE) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
                );

        if (cerrarPopupPressed) {
            cerrarPopup();
        }

        return;
    }

    if (IsGamepadAvailable(0)) {
        bool toggleMensajesApi =
                (
                    IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)
                ) ||
                (
                    IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)
                );

        if (toggleMensajesApi) {
            mostrarMensajesApi = !mostrarMensajesApi;
        }
    }

    if (
        screenTransitionAlpha > 0.0f &&
        shouldAnimateScreenTransition(currentScreen)
    ) {
        screenTransitionAlpha -= frameTime * 3.2f;

        if (screenTransitionAlpha < 0.0f) {
            screenTransitionAlpha = 0.0f;
        }
    }

    limpiarFinalizacionesPartidaTerminadas();

    if (currentScreen == CARGANDO) {
        actualizarCarga();

        if (
            currentScreen != screenBeforeUpdate &&
            shouldAnimateScreenTransition(currentScreen)
        ) {
            screenTransitionAlpha = 1.0f;
        }

        return;
    }

    switch (currentScreen) {
        case CARGA_INICIAL: {
            initialLoadTimer += frameTime;

            if (initialLoadTimer >= 1.45f) {
                currentScreen = INICIO;
            }

            break;
        }

        case INICIO: {
            menuPreviewTimer += frameTime;

            bgOffset -= 28.0f * frameTime;
            foregroundOffset -= 220.0f * frameTime;

            if (bgWidth > 0.0f && bgOffset <= -bgWidth) {
                bgOffset = 0.0f;
            }

            if (foregroundWidth > 0.0f && foregroundOffset <= -foregroundWidth) {
                foregroundOffset = 0.0f;
            }

            bool startPressed =
                    IsKeyPressed(KEY_ENTER) ||
                    IsKeyPressed(KEY_SPACE) ||
                    (
                        IsGamepadAvailable(0) &&
                        (
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT) ||
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
                        )
                    );

            if (startPressed) {
                currentScreen = MENU;
            }

            break;
        }

        case LOGIN: {
            std::string &campoActivo =
                    loginPasswordActivo ? loginPassword : loginUsername;

            int key = GetCharPressed();

            while (key > 0) {
                if (key >= 32 && key <= 126 && campoActivo.size() < 32) {
                    campoActivo.push_back(static_cast<char>(key));
                }

                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE) && !campoActivo.empty()) {
                campoActivo.pop_back();
            }

            if (
                IsKeyPressed(KEY_TAB) ||
                IsKeyPressed(KEY_UP) ||
                IsKeyPressed(KEY_DOWN)
            ) {
                loginPasswordActivo = !loginPasswordActivo;
            }

            if (
                IsKeyPressed(KEY_ESCAPE) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)
                )
            ) {
                currentScreen = MENU;
                break;
            }

            bool loginPressed =
                    IsKeyPressed(KEY_ENTER) ||
                    (
                        IsGamepadAvailable(0) &&
                        IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
                    );

            if (loginPressed) {
                if (!loginPasswordActivo) {
                    loginPasswordActivo = true;
                    break;
                }

                if (loginUsername.empty() || loginPassword.empty()) {
                    mostrarPopup(
                        "DATOS INCOMPLETOS",
                        "Ingresa usuario y contrasena para continuar.",
                        "ACEPTAR"
                    );
                    break;
                }

                iniciarCargaLogin();
            }

            break;
        }

        case MENU: {
            menuEasterEggTimer += frameTime;

            if (mensajeApi == "LOGIN: debes loguearte antes de jugar.") {
                bool abrirLoginPressed =
                        IsKeyPressed(KEY_TWO) ||
                        (
                            IsGamepadAvailable(0) &&
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)
                        );

                if (abrirLoginPressed) {
                    loginUsername.clear();
                    loginPassword.clear();
                    loginPasswordActivo = false;
                    currentScreen = LOGIN;
                }

                break;
            }

            if (
                gamepadBackPressed() ||
                IsKeyPressed(KEY_BACKSPACE)
            ) {
                currentScreen = INICIO;
                break;
            }

            if (
                IsKeyPressed(KEY_ONE) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
                )
            ) {
                if (!sesionIniciada && !modoPruebaSinApi) {
                    mensajeApi = "LOGIN: debes loguearte antes de jugar.";
                    break;
                }

                if (!modoPruebaSinApi && creditos < gameCost) {
                    mostrarPopup(
                        "TOKENS INSUFICIENTES",
                        TextFormat(
                            "Necesitas %i tokens para jugar. Tienes %i.",
                            gameCost,
                            creditos
                        ),
                        "ACEPTAR"
                    );
                    break;
                }

                iniciarCargaPartida(MENU, modoPruebaSinApi);
            }

            if (
                IsKeyPressed(KEY_TWO) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_UP)
                )
            ) {
                loginUsername.clear();
                loginPassword.clear();
                loginPasswordActivo = false;
                currentScreen = LOGIN;
            }

            if (
                IsKeyPressed(KEY_THREE) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)
                )
            ) {
                iniciarCargaRanking();
            }

            if (
                IsKeyPressed(KEY_FOUR) ||
                IsKeyPressed(KEY_H) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) &&
                    !IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)
                )
            ) {
                currentScreen = COMO_JUGAR;
            }

            if (
                IsKeyPressed(KEY_FIVE) ||
                (
                    IsGamepadAvailable(0) &&
                    IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)
                )
            ) {
                currentScreen = CONFIRMAR_SALIDA;
            }

            break;
        }

        case COMO_JUGAR: {
            menuPreviewTimer += frameTime;

            bool volverMenuPressed =
                    IsKeyPressed(KEY_ESCAPE) ||
                    IsKeyPressed(KEY_BACKSPACE) ||
                    IsKeyPressed(KEY_H) ||
                    (
                        IsGamepadAvailable(0) &&
                        (
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
                            (
                                IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) &&
                                !IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_TRIGGER_1)
                            ) ||
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)
                        )
                    );

            if (volverMenuPressed) {
                currentScreen = MENU;
            }

            break;
        }

        case RANKING: {
            bool volverMenuPressed =
                    IsKeyPressed(KEY_ESCAPE) ||
                    IsKeyPressed(KEY_R) ||
                    IsKeyPressed(KEY_ENTER) ||
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

        case CONFIRMAR_SALIDA: {
            audioManager.stopRunning();

            bool confirmarSalida =
                    IsKeyPressed(KEY_ENTER) ||
                    IsKeyPressed(KEY_Y) ||
                    (
                        IsGamepadAvailable(0) &&
                        IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
                    );

            if (confirmarSalida) {
                shouldCloseGame = true;
            }

            bool cancelarSalida =
                    IsKeyPressed(KEY_ESCAPE) ||
                    IsKeyPressed(KEY_N) ||
                    (
                        IsGamepadAvailable(0) &&
                        IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)
                    );

            if (cancelarSalida) {
                currentScreen = MENU;
            }

            break;
        }

        case JUGANDO: {
            bool pausaPressed =
                    IsKeyPressed(KEY_ESCAPE) ||
                    IsKeyPressed(KEY_P) ||
                    (
                        IsGamepadAvailable(0) &&
                        IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)
                    );

            if (pausaPressed) {
                currentScreen = PAUSA;
                break;
            }

            float deltaTime = GetFrameTime();

            if (nitroActive) {
                nitroTimer -= deltaTime;

                if (nitroTimer <= 0.0f) {
                    nitroTimer = 0.0f;
                    nitroActive = false;

                    if (player != nullptr) {
                        player->setNitro(false);
                    }
                }
            }

            if (hasShield) {
                shieldTimer -= deltaTime;

                if (shieldTimer <= 0.0f) {
                    shieldTimer = 0.0f;
                    hasShield = false;
                }
            }

            //scrolling imp

            float speedMultiplier = nitroActive ? 1.35f : 1.0f;
            float gameplaySpeed = globalSpeed * speedMultiplier;

            bgOffset -= 25.0f * deltaTime;

            foregroundOffset -= gameplaySpeed * 0.35f * deltaTime;

            if (bgOffset <= -bgWidth) {
                bgOffset = 0.0f;
            }

            if (foregroundOffset <= -foregroundWidth)
            {
                foregroundOffset = 0.0f;
            }

            if (transitionAlpha < 1.0f) {
                transitionAlpha += 0.5f * deltaTime;

                if (transitionAlpha > 1.0f) {
                    transitionAlpha = 1.0f;
                }
            }

            float scoreMultiplier = nitroActive ? 3.0f : 1.0f;

            scoreTimer += scorePerSecond * deltaTime * scoreMultiplier;

            score = static_cast<int>(scoreTimer);
            //reportarScoreApiSiCorresponde();

            globalSpeed += speedIncrement * deltaTime;

            float velocidadMaximaActual = maxNormalSpeed;

            if (globalSpeed > velocidadMaximaActual) {
                globalSpeed = velocidadMaximaActual;
            }

            gameplaySpeed = globalSpeed * speedMultiplier;

            if (player != nullptr) {
                player->update(deltaTime);
            }

            if (player->detectJumpStart())
            {
                audioManager.playJump();
            }

            if (player->detectDoubleJump())
            {
                audioManager.playDoubleJump();
            }

            if (player->detectLanding())
            {
                audioManager.playLanding();
            }

            if (player->isGrounded())
            {
                audioManager.startRunning();
            }
            else
            {
                audioManager.stopRunning();
            }

            for (auto &obs: obstacles) {
                obs.setSpeed(gameplaySpeed);

                obs.update(deltaTime);
            }

            separarObstaculos();


            for (auto &coin: coins) {
                coin.setSpeed(gameplaySpeed);

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
                        audioManager.playCoin();

                        int monedasGanadas = nitroActive ? 3 : 1;

                        coinsCollectedThisRun += monedasGanadas;

                        playerData.totalCoinsCollected += monedasGanadas;

                        scoreTimer += 25.0f * static_cast<float>(monedasGanadas);
                        score = static_cast<int>(scoreTimer);

                        dataManager.registerCoinCollected(
                            playerData.userId,
                            monedasGanadas
                        );
                    } else if (itemType == ItemType::NITRO) {
                        nitroActive = true;
                        nitroTimer = 8.0f;

                        if (player != nullptr) {
                            player->setNitro(true);
                        }

                        scoreTimer += 75.0f;
                        score = static_cast<int>(scoreTimer);
                    } else if (itemType == ItemType::SHIELD) {
                        hasShield = true;
                        shieldTimer = 10.0f;

                        audioManager.playShield();

                        scoreTimer += 50.0f * scoreMultiplier;
                        score = static_cast<int>(scoreTimer);
                    }

                    coin.collect();
                }
            }

            actualizarGeneracionMonedas();

            checkCollisions();

            break;
        }

        case PAUSA: {
            bool continuarPressed =
                    IsKeyPressed(KEY_ESCAPE) ||
                    IsKeyPressed(KEY_P) ||
                    IsKeyPressed(KEY_ONE) ||
                    (
                        IsGamepadAvailable(0) &&
                        (
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
                            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)
                        )
                    );

            if (continuarPressed) {
                currentScreen = JUGANDO;
            }

            bool salirMenuPressed =
                    IsKeyPressed(KEY_TWO) ||
                    (
                        IsGamepadAvailable(0) &&
                        IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)
                    );

            if (salirMenuPressed) {
                finalizarPartidaApiAsync("EXIT");
                iniciarCargaVolverMenu();
            }

            break;
        }

        case IMPACTO: {
            impactTimer += GetFrameTime();

            if (impactTimer >= 1.05f) {
                currentScreen = GAMEOVER;
            }

            break;
        }

        case GAMEOVER: {
            bool reiniciarPressed =
                    IsKeyPressed(KEY_ONE) ||
                    IsKeyPressed(KEY_R) ||
                    (
                        IsGamepadAvailable(0) &&
                        IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)
                    );

            if (reiniciarPressed) {
                iniciarCargaPartida(GAMEOVER, modoPruebaSinApi);

                break;
            }

            bool volverMenuPressed =
                    IsKeyPressed(KEY_TWO) ||
                    IsKeyPressed(KEY_ESCAPE) ||
                    (
                        IsGamepadAvailable(0) &&
                        IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)
                    );

            if (volverMenuPressed) {
                currentScreen = MENU;
                break;
            }

            break;
        }
    }

    if (
        currentScreen != screenBeforeUpdate &&
        shouldAnimateScreenTransition(currentScreen)
    ) {
        screenTransitionAlpha = 1.0f;
    }
}

void Game::checkCollisions()
{
    if (player == nullptr)
    {
        return;
    }

    for (auto &obs: obstacles)
    {
        if (
            CheckCollisionRecs(
                player->getRect(),
                obs.getRect()
            )
        )
        {
            if (obs.getType() == ObstacleType::AIR)
            {
                audioManager.playDroneImpact();
            }
            else
            {
                audioManager.playBoxImpact();
            }

            if (hasShield)
            {
                hasShield = false;
                shieldTimer = 0.0f;

                obs.forceRespawn();

                return;
            }

            audioManager.stopRunning();

            audioManager.playGameOver();

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

            impactPosition = player->getPosition();
            impactTimer = 0.0f;

            finalizarPartidaApiAsync("LOSE");

            currentScreen = IMPACTO;

            return;
        }
    }
}

void Game::drawLoadingScreen()
{
    drawCyberPanel(210, 140, 380, 175, NEO_CYAN);

    int puntos = static_cast<int>(GetTime() * 4.0) % 4;
    std::string titulo = tituloCarga;

    for (int i = 0; i < puntos; ++i) {
        titulo += ".";
    }

    int tituloWidth = MeasureText(titulo.c_str(), 24);

    drawCyberText(
        titulo.c_str(),
        400 - tituloWidth / 2,
        170,
        24,
        NEO_CYAN
    );

    int detalleWidth = MeasureText(detalleCarga.c_str(), 15);

    drawCyberText(
        detalleCarga.c_str(),
        400 - detalleWidth / 2,
        210,
        15,
        LIGHTGRAY
    );

    float progreso = static_cast<float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - inicioCarga
        ).count()
    ) / 900.0f;

    if (progreso > 1.0f) {
        progreso = 1.0f;
    }

    DrawRectangle(260, 255, 280, 12, {0, 0, 0, 220});
    DrawRectangleLinesEx({260.0f, 255.0f, 280.0f, 12.0f}, 1.5f, NEO_MAGENTA);
    DrawRectangle(262, 257, static_cast<int>(276.0f * progreso), 8, NEO_CYAN);

    drawCyberText(
        "Conectando con servicios del juego",
        278,
        282,
        13,
        GRAY
    );
}

void Game::drawImpactAnimation()
{
    int frame = static_cast<int>(impactTimer / 0.16f);

    if (frame > 3) {
        frame = 3;
    }

    Texture2D texture = impactTextures[frame];

    if (texture.id) {
        Rectangle source = {
            0.0f,
            0.0f,
            static_cast<float>(texture.width),
            static_cast<float>(texture.height)
        };

        Rectangle dest = {
            impactPosition.x - 48.0f,
            impactPosition.y - 52.0f,
            96.0f,
            96.0f
        };

        DrawTexturePro(
            texture,
            source,
            dest,
            {0.0f, 0.0f},
            0.0f,
            WHITE
        );
    } else {
        DrawCircleGradient(
            static_cast<int>(impactPosition.x),
            static_cast<int>(impactPosition.y),
            46.0f,
            NEO_RED,
            {0, 0, 0, 0}
        );
    }

    float fadeProgress = (impactTimer - 0.55f) / 0.50f;

    if (fadeProgress < 0.0f) {
        fadeProgress = 0.0f;
    }

    if (fadeProgress > 1.0f) {
        fadeProgress = 1.0f;
    }

    DrawRectangle(
        0,
        0,
        screenWidth,
        screenHeight,
        {0, 0, 0, static_cast<unsigned char>(fadeProgress * 205.0f)}
    );
}

void Game::drawGame() {
    switch (currentScreen) {
        case CARGA_INICIAL: {
            audioManager.stopRunning();

            drawCyberPanel(205, 135, 390, 185, NEO_CYAN);

            int puntos = static_cast<int>(GetTime() * 4.0) % 4;
            std::string titulo = "INICIANDO SISTEMA";

            for (int i = 0; i < puntos; ++i) {
                titulo += ".";
            }

            int tituloWidth = MeasureText(titulo.c_str(), 24);

            drawCyberText(
                titulo.c_str(),
                400 - tituloWidth / 2,
                168,
                24,
                NEO_CYAN
            );

            drawCyberText(
                "Preparando interfaz neural",
                300,
                210,
                16,
                LIGHTGRAY
            );

            float progreso = initialLoadTimer / 1.45f;

            if (progreso > 1.0f) {
                progreso = 1.0f;
            }

            DrawRectangle(260, 255, 280, 12, {0, 0, 0, 220});
            DrawRectangleLinesEx(
                {260.0f, 255.0f, 280.0f, 12.0f},
                1.5f,
                NEO_MAGENTA
            );
            DrawRectangle(
                262,
                257,
                static_cast<int>(276.0f * progreso),
                8,
                NEO_CYAN
            );

            break;
        }

        case INICIO: {
            audioManager.stopRunning();

            const float previewSpeed = 245.0f;
            const float previewPlayerX = 105.0f;
            auto previewCollected = [previewPlayerX](float x, float width) {
                return x + width > previewPlayerX + 8.0f &&
                       x < previewPlayerX + 54.0f;
            };

            float coinX = getMenuPreviewX(520.0f, previewSpeed, menuPreviewTimer);
            float coinMatrixX = getMenuPreviewX(700.0f, previewSpeed, menuPreviewTimer);
            float obstacleX = getMenuPreviewX(900.0f, previewSpeed, menuPreviewTimer);
            float droneX = getMenuPreviewX(1120.0f, previewSpeed, menuPreviewTimer);
            float droneY =
                    145.0f +
                    std::sin(menuPreviewTimer * 2.2f) * 18.0f;
            float nitroX = getMenuPreviewX(1320.0f, previewSpeed, menuPreviewTimer);
            float obstacleX2 = getMenuPreviewX(1510.0f, previewSpeed, menuPreviewTimer);
            float droneX2 = getMenuPreviewX(1710.0f, previewSpeed, menuPreviewTimer);
            float droneY2 =
                    205.0f +
                    std::sin(menuPreviewTimer * 2.8f + 1.7f) * 14.0f;
            float shieldX = getMenuPreviewX(1890.0f, previewSpeed, menuPreviewTimer);
            auto getObstacleJumpHeight = [previewPlayerX](float x) {
                float distance = x - previewPlayerX;

                if (distance < -88.0f || distance > 210.0f) {
                    return 0.0f;
                }

                float normalized = (distance + 88.0f) / 298.0f;
                float firstJump =
                        std::sin(normalized * 3.14159265f) *
                        62.0f;
                float secondJump =
                        std::sin(normalized * 6.28318530f) *
                        34.0f;

                if (secondJump < 0.0f) {
                    secondJump = 0.0f;
                }

                return firstJump + secondJump;
            };
            float jumpHeight = std::max(
                getObstacleJumpHeight(obstacleX),
                getObstacleJumpHeight(obstacleX2)
            );
            bool previewDoubleJumping = jumpHeight > 82.0f;
            bool previewJumping = jumpHeight > 5.0f;
            Texture2D previewPlayerTexture =
                    previewDoubleJumping
                        ? menuPlayerDoubleJump
                        : (
                            previewJumping
                                ? menuPlayerJump
                                : menuPlayerRun[
                                    static_cast<int>(menuPreviewTimer * 9.0f) % 3
                                ]
                        );
            Rectangle previewPlayer = {
                previewPlayerX,
                292.0f - jumpHeight,
                38.0f,
                66.0f
            };

            DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 55});
            DrawLine(70, 352, 730, 352, {0, 255, 255, 80});
            DrawLine(90, 356, 520, 356, {253, 249, 0, 70});

            drawMenuPreviewTexture(
                menuDroneTexture,
                {droneX, droneY, 62.0f, 38.0f},
                {255, 255, 255, 170},
                NEO_YELLOW
            );
            drawMenuPreviewTexture(
                menuDroneTexture,
                {droneX2, droneY2, 52.0f, 32.0f},
                {255, 255, 255, 135},
                NEO_YELLOW
            );
            drawMenuPreviewTexture(
                menuObstacleTexture,
                {obstacleX, 305.0f, 38.0f, 53.0f},
                {255, 255, 255, 155},
                NEO_MAGENTA
            );
            drawMenuPreviewTexture(
                menuObstacleTexture,
                {obstacleX2, 307.0f, 34.0f, 49.0f},
                {255, 255, 255, 120},
                NEO_MAGENTA
            );
            DrawCircleGradient(
                120,
                static_cast<int>(340.0f - jumpHeight),
                42.0f,
                {0, 255, 255, 80},
                {0, 0, 0, 0}
            );
            if (!previewCollected(coinX, 22.0f)) {
                drawMenuPreviewTexture(
                    menuCoinTexture[
                        static_cast<int>(menuPreviewTimer * 8.0f) % 2
                    ],
                    {coinX, 245.0f, 22.0f, 22.0f},
                    {255, 255, 255, 170},
                    NEO_YELLOW
                );
            }
            for (int i = 0; i < 4; ++i) {
                float matrixCoinX =
                        coinMatrixX + static_cast<float>(i % 2) * 28.0f;

                if (!previewCollected(matrixCoinX, 20.0f)) {
                    drawMenuPreviewTexture(
                        menuCoinTexture[
                            (i + static_cast<int>(menuPreviewTimer * 8.0f)) % 2
                        ],
                        {
                            matrixCoinX,
                            230.0f + static_cast<float>(i / 2) * 28.0f,
                            20.0f,
                            20.0f
                        },
                        {255, 255, 255, 150},
                        NEO_YELLOW
                    );
                }
            }
            if (!previewCollected(nitroX, 30.0f)) {
                drawMenuPreviewTexture(
                    menuNitroTexture,
                    {nitroX, 258.0f, 30.0f, 30.0f},
                    {255, 255, 255, 165},
                    NEO_CYAN
                );
            }
            if (!previewCollected(shieldX, 32.0f)) {
                drawMenuPreviewTexture(
                    menuShieldTexture,
                    {shieldX, 220.0f, 32.0f, 32.0f},
                    {255, 255, 255, 155},
                    NEO_CYAN
                );
            }
            drawMenuPreviewTexture(
                previewPlayerTexture,
                previewPlayer,
                {255, 255, 255, 190},
                NEO_RED
            );

            DrawText("CYBER", 280, 122, 52, {0, 0, 0, 210});
            DrawText("CYBER", 276, 118, 52, {255, 0, 255, 150});
            DrawText("CYBER", 272, 114, 52, {0, 255, 255, 150});
            DrawText("CYBER", 274, 116, 52, NEO_YELLOW);

            DrawText("RUNNER", 315, 183, 38, {0, 0, 0, 220});
            DrawText("RUNNER", 311, 179, 38, {255, 0, 255, 175});
            DrawText("RUNNER", 307, 175, 38, {0, 255, 255, 175});
            DrawText("RUNNER", 309, 177, 38, WHITE);

            DrawLine(252, 174, 382, 174, {0, 0, 0, 210});
            DrawLine(252, 170, 382, 170, NEO_MAGENTA);
            DrawLine(400, 160, 500, 160, {0, 0, 0, 210});
            DrawLine(400, 156, 500, 156, NEO_CYAN);
            DrawLine(318, 220, 454, 220, {0, 0, 0, 210});
            DrawLine(318, 216, 454, 216, NEO_YELLOW);

            bool mostrarStart = static_cast<int>(GetTime() * 2.0) % 2 == 0;

            if (mostrarStart) {
                drawCenteredCyberText(
                    "PRESIONA START",
                    400,
                    305,
                    25,
                    NEO_YELLOW
                );
            }

            DrawRectangle(324, 352, 152, 1, {0, 255, 255, 120});
            drawRetroPillButton(
                {295.0f, 368.0f, 76.0f, 28.0f},
                "START",
                NEO_CYAN
            );
            drawRetroRoundButton(400.0f, 382.0f, "A", NEO_YELLOW);
            drawCyberText(
                "COMENZAR",
                425,
                374,
                14,
                LIGHTGRAY
            );

            break;
        }

        case LOGIN: {
            drawCyberPanel(185, 60, 430, 335, NEO_CYAN);

            drawCyberText(
                "CYBER-RUNNER",
                250,
                80,
                35,
                NEO_CYAN
            );

            drawCyberText(
                "LOGIN",
                360,
                135,
                24,
                NEO_MAGENTA
            );

            drawCyberText(
                "Usuario",
                250,
                158,
                16,
                LIGHTGRAY
            );

            DrawRectangle(250, 180, 300, 38, {0, 0, 0, 225});
            DrawRectangleLinesEx(
                {250.0f, 180.0f, 300.0f, 38.0f},
                2.0f,
                loginPasswordActivo ? WHITE : NEO_CYAN
            );

            drawCyberText(
                loginUsername.empty() ? "_" : loginUsername.c_str(),
                262,
                190,
                18,
                WHITE
            );

            drawCyberText(
                "Password",
                250,
                238,
                16,
                LIGHTGRAY
            );

            DrawRectangle(250, 260, 300, 38, {0, 0, 0, 225});
            DrawRectangleLinesEx(
                {250.0f, 260.0f, 300.0f, 38.0f},
                2.0f,
                loginPasswordActivo ? NEO_CYAN : WHITE
            );

            std::string passwordOculto(loginPassword.size(), '*');

            drawCyberText(
                loginPassword.empty() ? "_" : passwordOculto.c_str(),
                262,
                270,
                18,
                WHITE
            );

            drawRetroRoundButton(285.0f, 340.0f, "A", NEO_CYAN);
            drawCyberText("LOGIN", 310, 332, 14, LIGHTGRAY);
            drawRetroRoundButton(385.0f, 340.0f, "B", NEO_RED);
            drawCyberText("MENU", 410, 332, 14, LIGHTGRAY);

            if (mostrarMensajesApi) {
                drawCyberText(
                    mensajeApi.c_str(),
                    250,
                    365,
                    20,
                    LIGHTGRAY
                );
            }

            break;
        }

        case MENU: {
            audioManager.stopRunning();

            drawCyberPanel(185, 25, 430, 395, NEO_MAGENTA);

            float easterPhase = std::fmod(menuEasterEggTimer, 24.0f);

            if (easterPhase < 5.8f) {
                float runProgress = easterPhase / 5.8f;
                float runnerX = -80.0f + runProgress * 960.0f;
                float jumpPulse =
                        std::sin(runProgress * 3.14159265f * 2.0f);

                if (jumpPulse < 0.0f) {
                    jumpPulse = 0.0f;
                }

                float runnerJump = jumpPulse * 34.0f;
                bool runnerJumping = runnerJump > 4.0f;
                Texture2D runnerTexture =
                        runnerJumping
                            ? menuPlayerJump
                            : menuPlayerRun[
                                static_cast<int>(menuEasterEggTimer * 11.0f) % 3
                            ];

                drawMenuPreviewTexture(
                    runnerTexture,
                    {runnerX, 306.0f - runnerJump, 34.0f, 64.0f},
                    {255, 255, 255, 150},
                    NEO_RED
                );
            }

            drawCyberText(
                "ACCESO CONCEDIDO",
                300,
                40,
                20,
                GREEN
            );

            drawCyberText(
                "TERMINAL DE CONTROL",
                238,
                80,
                25,
                NEO_MAGENTA
            );

            drawCyberText(
                sesionIniciada
                    ? TextFormat("JUGADOR: %s", playerName.c_str())
                    : "JUGADOR: sin login",
                250,
                115,
                16,
                sesionIniciada ? NEO_YELLOW : GRAY
            );

            drawRetroActionButton(
                {250.0f, 140.0f, 300.0f, 38.0f},
                "A",
                "EMPEZAR PARTIDA",
                NEO_CYAN
            );

            drawRetroActionButton(
                {250.0f, 190.0f, 300.0f, 38.0f},
                "Y",
                sesionIniciada ? "CAMBIAR USUARIO" : "LOGUEARSE",
                WHITE
            );

            drawRetroActionButton(
                {250.0f, 240.0f, 300.0f, 38.0f},
                "X",
                "VER RANKING",
                NEO_YELLOW
            );

            drawRetroActionButton(
                {250.0f, 290.0f, 300.0f, 38.0f},
                "RB",
                "COMO JUGAR",
                NEO_CYAN
            );

            drawRetroActionButton(
                {250.0f, 340.0f, 300.0f, 38.0f},
                "B",
                "SALIR DEL JUEGO",
                NEO_RED
            );

            drawCyberText(
                TextFormat("COSTO POR PARTIDA: %i TOKEN(S)", gameCost),
                250,
                388,
                13,
                NEO_YELLOW
            );

            if (mostrarMensajesApi) {
                drawCyberText(
                    mensajeApi.c_str(),
                    250,
                    405,
                    13,
                    LIGHTGRAY
                );
            }

            hud.drawMenuHUD(creditos);

            if (mensajeApi == "LOGIN: debes loguearte antes de jugar.") {
                DrawRectangle(165, 165, 470, 92, {0, 0, 0, 235});
                DrawRectangleLinesEx({165.0f, 165.0f, 470.0f, 92.0f}, 2.0f, NEO_RED);
                DrawRectangleLines(175, 175, 450, 72, {255, 255, 255, 75});

                drawCenteredCyberText(
                    "DEBES LOGUEARTE ANTES DE JUGAR",
                    400,
                    188,
                    19,
                    NEO_YELLOW
                );
                drawCyberText("PRESIONA", 282, 218, 16, LIGHTGRAY);
                drawRetroRoundButton(386.0f, 226.0f, "Y", WHITE);
                drawCyberText("PARA LOGUEARTE", 412, 218, 16, LIGHTGRAY);
            }

            break;
        }

        case COMO_JUGAR: {
            audioManager.stopRunning();

            drawCyberPanel(70, 25, 660, 395, NEO_CYAN);

            drawCenteredCyberText(
                "COMO JUGAR",
                400,
                42,
                32,
                NEO_CYAN
            );

            DrawRectangle(105, 92, 260, 250, {0, 0, 0, 225});
            DrawRectangleLinesEx({105.0f, 92.0f, 260.0f, 250.0f}, 1.5f, NEO_MAGENTA);
            DrawLine(120, 190, 350, 190, {0, 255, 255, 100});

            Texture2D runnerTexture =
                    (static_cast<int>(menuPreviewTimer * 8.0f) % 2 == 0)
                        ? menuPlayerRun[static_cast<int>(menuPreviewTimer * 9.0f) % 3]
                        : menuPlayerJump;

            drawMenuPreviewTexture(
                runnerTexture,
                {132.0f, 230.0f, 36.0f, 64.0f},
                WHITE,
                NEO_CYAN
            );

            drawCenteredCyberText("OBSTACULOS", 235, 108, 17, NEO_MAGENTA);

            drawMenuPreviewTexture(
                menuObstacleTexture,
                {150.0f, 132.0f, 38.0f, 52.0f},
                WHITE,
                NEO_MAGENTA
            );

            float instructionDroneY =
                    135.0f +
                    std::sin(menuPreviewTimer * 2.2f) * 12.0f;

            drawMenuPreviewTexture(
                menuDroneTexture,
                {240.0f, instructionDroneY, 58.0f, 36.0f},
                WHITE,
                NEO_YELLOW
            );

            drawCenteredCyberText("RECOGIBLES", 235, 208, 17, NEO_YELLOW);

            drawMenuPreviewTexture(
                menuCoinTexture[static_cast<int>(menuPreviewTimer * 8.0f) % 2],
                {205.0f, 228.0f, 25.0f, 25.0f},
                WHITE,
                NEO_YELLOW
            );

            drawMenuPreviewTexture(
                menuNitroTexture,
                {248.0f, 226.0f, 30.0f, 30.0f},
                WHITE,
                NEO_CYAN
            );

            drawMenuPreviewTexture(
                menuShieldTexture,
                {292.0f, 224.0f, 32.0f, 32.0f},
                WHITE,
                NEO_CYAN
            );

            drawCenteredCyberText("EVITA LOS OBSTACULOS", 235, 303, 14, LIGHTGRAY);
            drawCenteredCyberText("TOMA MONEDAS, NITRO Y ESCUDOS", 235, 320, 13, LIGHTGRAY);

            drawRetroRoundButton(435.0f, 120.0f, "A", NEO_CYAN);
            drawCyberText("SALTAR", 475, 112, 18, WHITE);

            drawRetroRoundButton(435.0f, 170.0f, "A", NEO_YELLOW);
            drawRetroRoundButton(475.0f, 170.0f, "A", NEO_YELLOW);
            drawCyberText("DOBLE SALTO", 520, 162, 18, WHITE);

            drawRetroRoundButton(435.0f, 220.0f, "", NEO_MAGENTA);
            DrawTriangle(
                {435.0f, 230.0f},
                {424.0f, 211.0f},
                {446.0f, 211.0f},
                {0, 0, 0, 210}
            );
            DrawTriangle(
                {435.0f, 227.0f},
                {425.0f, 212.0f},
                {445.0f, 212.0f},
                WHITE
            );
            DrawTriangleLines(
                {435.0f, 227.0f},
                {425.0f, 212.0f},
                {445.0f, 212.0f},
                NEO_CYAN
            );
            drawCyberText("CAIDA RAPIDA", 475, 212, 18, WHITE);

            drawRetroPillButton({405.0f, 255.0f, 78.0f, 30.0f}, "START", NEO_RED);
            drawCyberText("PAUSA", 505, 262, 18, WHITE);

            drawCenteredCyberText(
                "EL NITRO AUMENTA LA DIFICULTAD TEMPORALMENTE",
                400,
                346,
                13,
                NEO_YELLOW
            );
            drawCenteredCyberText(
                "PERO TRIPLICA EL VALOR DE TOKENS Y SCORE",
                400,
                362,
                13,
                NEO_YELLOW
            );

            drawRetroRoundButton(260.0f, 395.0f, "B", NEO_RED);
            drawCyberText("VOLVER AL MENU", 290, 387, 16, LIGHTGRAY);

            break;
        }

        case RANKING: {
            audioManager.stopRunning();

            drawCyberPanel(80, 30, 640, 385, NEO_CYAN);

            drawCyberText(
                "RANKING",
                330,
                45,
                32,
                NEO_CYAN
            );

            DrawRectangle(110, 95, 580, 265, {0, 0, 0, 225});

            DrawRectangleLinesEx(
                {110.0f, 95.0f, 580.0f, 265.0f},
                1.5f,
                NEO_MAGENTA
            );

            DrawRectangle(110, 95, 580, 42, {0, 255, 255, 35});

            drawCyberText(
                "#",
                135,
                115,
                18,
                NEO_YELLOW
            );

            drawCyberText(
                "JUGADOR",
                190,
                115,
                18,
                NEO_YELLOW
            );

            drawCyberText(
                "SCORE",
                520,
                115,
                18,
                NEO_YELLOW
            );

            if (rankingActual.empty()) {
                if (mostrarMensajesApi) {
                    drawCyberText(
                        mensajeApi.c_str(),
                        210,
                        215,
                        20,
                        GRAY
                    );
                }
            } else {
                int puestoUsuario = -1;
                int scoreUsuario = 0;

                for (size_t i = 0; i < rankingActual.size(); ++i) {
                    if (rankingActual[i].username == playerName) {
                        puestoUsuario = static_cast<int>(i + 1);
                        scoreUsuario = rankingActual[i].bestScore;
                        break;
                    }
                }

                int filas = std::min(
                    static_cast<int>(rankingActual.size()),
                    7
                );

                for (int i = 0; i < filas; ++i) {
                    int y = 148 + i * 24;
                    std::string usernameRanking = rankingActual[i].username;
                    bool esUsuarioActual = usernameRanking == playerName;

                    if (esUsuarioActual) {
                        DrawRectangle(120, y - 4, 560, 23, {0, 255, 255, 45});
                        DrawRectangleLines(120, y - 4, 560, 23, NEO_CYAN);
                    } else if (i % 2 == 0) {
                        DrawRectangle(120, y - 4, 560, 23, {255, 255, 255, 18});
                    }

                    if (usernameRanking.size() > 18) {
                        usernameRanking = usernameRanking.substr(0, 15) + "...";
                    }

                    if (i == 0) {
                        int crownX = 143;
                        int crownY = y - 4;
                        int pixel = 3;
                        int shineStep = static_cast<int>(GetTime() * 6.0f) % 5;
                        Color crownShadow = {0, 0, 0, 180};
                        Color crownGold = NEO_YELLOW;
                        Color crownDeep = {224, 155, 20, 255};
                        Color crownShine = {255, 255, 255, 230};

                        DrawRectangle(crownX - 2, crownY - 2, 37, 21, crownShadow);

                        DrawRectangle(crownX, crownY + pixel * 4, pixel * 11, pixel * 3, crownGold);
                        DrawRectangle(crownX + pixel, crownY + pixel * 3, pixel * 2, pixel, crownGold);
                        DrawRectangle(crownX + pixel * 4, crownY + pixel * 2, pixel * 3, pixel * 2, crownGold);
                        DrawRectangle(crownX + pixel * 8, crownY + pixel * 3, pixel * 2, pixel, crownGold);
                        DrawRectangle(crownX + pixel, crownY + pixel * 6, pixel * 9, pixel, crownDeep);

                        DrawRectangle(crownX, crownY + pixel * 2, pixel, pixel * 3, crownGold);
                        DrawRectangle(crownX + pixel * 2, crownY, pixel, pixel * 4, crownGold);
                        DrawRectangle(crownX + pixel * 5, crownY - pixel, pixel, pixel * 5, crownGold);
                        DrawRectangle(crownX + pixel * 8, crownY, pixel, pixel * 4, crownGold);
                        DrawRectangle(crownX + pixel * 10, crownY + pixel * 2, pixel, pixel * 3, crownGold);

                        DrawRectangle(crownX + pixel * 2, crownY - pixel, pixel, pixel, crownShine);
                        DrawRectangle(crownX + pixel * 5, crownY - pixel * 2, pixel, pixel, crownShine);
                        DrawRectangle(crownX + pixel * 8, crownY - pixel, pixel, pixel, crownShine);

                        DrawRectangle(
                            crownX + pixel * (1 + shineStep * 2),
                            crownY + pixel * 4,
                            pixel,
                            pixel,
                            crownShine
                        );
                        DrawRectangleLines(crownX - 1, crownY + pixel * 4 - 1, pixel * 11 + 2, pixel * 3 + 2, {255, 255, 255, 90});
                    } else {
                        drawCyberText(
                            TextFormat("%i", i + 1),
                            145,
                            y,
                            18,
                            WHITE
                        );
                    }

                    drawCyberText(
                        usernameRanking.c_str(),
                        190,
                        y,
                        18,
                        WHITE
                    );

                    drawCyberText(
                        TextFormat("%i", rankingActual[i].bestScore),
                        520,
                        y,
                        18,
                        WHITE
                    );
                }

                DrawLine(120, 318, 680, 318, {0, 255, 255, 90});
                DrawRectangle(120, 325, 560, 28, {0, 0, 0, 190});
                DrawRectangleLines(120, 325, 560, 28, NEO_YELLOW);

                if (puestoUsuario > 0) {
                    std::string nombreUsuario = playerName;

                    if (nombreUsuario.size() > 13) {
                        nombreUsuario = nombreUsuario.substr(0, 10) + "...";
                    }

                    drawCyberText("TU PUESTO", 135, 333, 14, NEO_CYAN);
                    drawCyberText(
                        TextFormat("#%i", puestoUsuario),
                        265,
                        333,
                        14,
                        NEO_YELLOW
                    );
                    drawCyberText(nombreUsuario.c_str(), 325, 333, 14, WHITE);
                    drawCyberText(
                        TextFormat("%i", scoreUsuario),
                        555,
                        333,
                        14,
                        WHITE
                    );
                } else {
                    drawCenteredCyberText(
                        "TU PUESTO: SIN REGISTRO EN RANKING",
                        400,
                        333,
                        14,
                        LIGHTGRAY
                    );
                }
            }

            drawRetroRoundButton(250.0f, 392.0f, "A", NEO_CYAN);
            drawRetroRoundButton(310.0f, 392.0f, "B", NEO_RED);
            drawCyberText("VOLVER AL MENU", 340, 384, 16, LIGHTGRAY);

            break;
        }

        case CARGANDO: {
            audioManager.stopRunning();
            drawLoadingScreen();
            break;
        }

        case CONFIRMAR_SALIDA: {
            drawCyberText(
                "CYBER RUNNER",
                245,
                80,
                40,
                NEO_CYAN
            );

            drawCyberPanel(170, 145, 460, 170, NEO_RED);

            drawCyberText(
                "CONFIRMAR SALIDA",
                255,
                170,
                28,
                NEO_YELLOW
            );

            drawCyberText(
                "Seguro que quieres cerrar el juego?",
                215,
                220,
                20,
                WHITE
            );

            drawRetroRoundButton(250.0f, 275.0f, "A", NEO_CYAN);
            drawCyberText("SI", 275, 267, 18, NEO_CYAN);
            drawRetroRoundButton(420.0f, 275.0f, "B", NEO_RED);
            drawCyberText("NO", 445, 267, 18, NEO_RED);

            break;
        }

        case JUGANDO: {
            if (nitroActive && player != nullptr) {
                Rectangle playerRect = player->getRect();
                float pulse =
                        (std::sin(static_cast<float>(GetTime()) * 16.0f) + 1.0f) *
                        0.5f;
                float lineStartX = playerRect.x - 74.0f - pulse * 8.0f;
                float lineEndX = playerRect.x - 10.0f;

                DrawLineEx(
                    {lineStartX, playerRect.y + 12.0f},
                    {lineEndX, playerRect.y + 12.0f},
                    3.0f,
                    {253, 249, 0, 165}
                );
                DrawLineEx(
                    {lineStartX - 12.0f, playerRect.y + 30.0f},
                    {lineEndX, playerRect.y + 30.0f},
                    4.0f,
                    {0, 255, 255, 170}
                );
                DrawLineEx(
                    {lineStartX - 4.0f, playerRect.y + 48.0f},
                    {lineEndX, playerRect.y + 48.0f},
                    2.0f,
                    {255, 255, 255, 135}
                );
            }

            if (player != nullptr) {
                player->draw();
            }

            for (auto &obs: obstacles) {
                obs.draw();
            }

            for (auto &coin: coins) {
                coin.draw();
            }

            //DrawLine(
            //    0,
            //    350,
            //    800,
            //    350,
            //    NEO_MAGENTA
            //);

            hud.drawGameHUD(
                nitroActive ? globalSpeed * 1.35f : globalSpeed,
                score,
                highScore,
                nitroActive,
                hasShield,
                nitroTimer / 8.0f,
                shieldTimer / 10.0f
            );

            DrawCircleGradient(682, 410, 34.0f, {0, 0, 0, 135}, {0, 0, 0, 0});
            drawMenuPreviewTexture(
                menuCoinTexture[static_cast<int>(GetTime() * 8.0f) % 2],
                {642.0f, 398.0f, 20.0f, 20.0f},
                WHITE,
                NEO_YELLOW
            );

            DrawText("x", 667, 400, 16, {0, 0, 0, 210});
            DrawText(TextFormat("%i", coinsCollectedThisRun), 690, 400, 16, {0, 0, 0, 210});
            drawCyberText("x", 666, 399, 16, LIGHTGRAY);
            drawCyberText(
                TextFormat("%i", coinsCollectedThisRun),
                689,
                399,
                16,
                WHITE
            );

            if (hasShield) {
                if (player != nullptr) {
                    Vector2 playerCenter = player->getPosition();
                    float time = static_cast<float>(GetTime());
                    float pulse = (std::sin(time * 5.5f) + 1.0f) * 0.5f;
                    float radius = 38.0f + pulse * 5.0f;

                    DrawCircleGradient(
                        static_cast<int>(playerCenter.x),
                        static_cast<int>(playerCenter.y),
                        radius + 12.0f,
                        {0, 140, 255, 90},
                        {0, 0, 0, 0}
                    );

                    DrawRing(
                        playerCenter,
                        radius,
                        radius + 3.0f,
                        time * 95.0f,
                        time * 95.0f + 275.0f,
                        72,
                        {0, 210, 255, 190}
                    );
                    DrawRing(
                        playerCenter,
                        radius - 7.0f,
                        radius - 4.0f,
                        -time * 130.0f,
                        -time * 130.0f + 135.0f,
                        72,
                        {253, 249, 0, 150}
                    );
                    DrawRing(
                        playerCenter,
                        radius + 5.0f,
                        radius + 6.5f,
                        0.0f,
                        360.0f,
                        72,
                        {255, 255, 255, 70}
                    );
                }
            }

            break;
        }

        case IMPACTO: {
            drawImpactAnimation();

            break;
        }

        case PAUSA: {
            audioManager.stopRunning();
            DrawRectangle(
                0,
                0,
                screenWidth,
                screenHeight,
                {0, 0, 0, 180}
            );

            drawCyberPanel(210, 100, 380, 255, NEO_YELLOW);

            drawCyberText(
                "JUEGO EN PAUSA",
                255,
                120,
                30,
                NEO_YELLOW
            );

            drawRetroActionButton(
                {230.0f, 190.0f, 340.0f, 45.0f},
                "A",
                "CONTINUAR",
                NEO_CYAN
            );

            drawRetroActionButton(
                {230.0f, 255.0f, 340.0f, 45.0f},
                "B",
                "SALIR AL MENU",
                NEO_RED
            );

            break;
        }

        case GAMEOVER: {
            audioManager.stopRunning();
            drawCyberPanel(145, 55, 510, 340, NEO_RED);

            int tokensResumen = calcularTokensGanados(score);
            const char *tokenLabel = tokensResumen == 1 ? "TOKEN" : "TOKENS";
            Rectangle scoreBox = {185.0f, 120.0f, 205.0f, 74.0f};
            Rectangle recordBox = {410.0f, 120.0f, 205.0f, 74.0f};
            Rectangle conversionBox = {185.0f, 205.0f, 430.0f, 94.0f};

            drawCenteredCyberText(
                "RESUMEN DE PARTIDA",
                400,
                78,
                29,
                NEO_RED
            );

            DrawRectangleRec(scoreBox, {0, 0, 0, 175});
            DrawRectangleLinesEx(scoreBox, 2.0f, NEO_CYAN);
            DrawRectangleLines(192, 127, 191, 60, {255, 255, 255, 55});
            drawMovingBorderGlow(scoreBox, NEO_CYAN, 0.0f);

            drawCenteredCyberText(
                "PUNTAJE",
                static_cast<int>(scoreBox.x + scoreBox.width / 2.0f),
                132,
                14,
                NEO_CYAN
            );

            drawCenteredCyberText(
                TextFormat("%i", score),
                static_cast<int>(scoreBox.x + scoreBox.width / 2.0f),
                154,
                24,
                WHITE
            );

            DrawRectangleRec(recordBox, {0, 0, 0, 175});
            DrawRectangleLinesEx(recordBox, 2.0f, GREEN);
            DrawRectangleLines(417, 127, 191, 60, {255, 255, 255, 55});
            drawMovingBorderGlow(recordBox, GREEN, 0.34f);

            drawCenteredCyberText(
                "RECORD",
                static_cast<int>(recordBox.x + recordBox.width / 2.0f),
                132,
                14,
                GREEN
            );

            drawCenteredCyberText(
                TextFormat("%i", highScore),
                static_cast<int>(recordBox.x + recordBox.width / 2.0f),
                154,
                24,
                WHITE
            );

            DrawRectangleRec(conversionBox, {0, 0, 0, 185});
            DrawRectangleLinesEx(conversionBox, 2.0f, NEO_YELLOW);
            DrawRectangleLines(192, 212, 416, 80, {0, 255, 255, 75});
            DrawLine(400, 229, 400, 263, {255, 255, 255, 75});
            DrawLine(210, 271, 590, 271, {253, 249, 0, 75});
            drawMovingBorderGlow(conversionBox, NEO_YELLOW, 0.68f);

            drawCenteredCyberText(
                "MONEDAS",
                294,
                216,
                13,
                NEO_YELLOW
            );

            DrawCircle(236, 246, 16.0f, {253, 249, 0, 45});
            DrawCircleLines(236, 246, 16.0f, NEO_YELLOW);
            drawCyberText("C", 230, 236, 20, NEO_YELLOW);

            drawCenteredCyberText(
                TextFormat("%i", coinsCollectedThisRun),
                294,
                238,
                25,
                WHITE
            );

            drawCenteredCyberText("=", 400, 238, 24, LIGHTGRAY);

            drawCenteredCyberText(
                tokenLabel,
                506,
                216,
                13,
                NEO_CYAN
            );

            DrawCircle(448, 246, 16.0f, {0, 255, 255, 45});
            DrawCircleLines(448, 246, 16.0f, NEO_CYAN);
            drawCyberText("T", 442, 236, 20, NEO_CYAN);

            drawCenteredCyberText(
                TextFormat("%i", tokensResumen),
                506,
                238,
                25,
                WHITE
            );

            drawCenteredCyberText(
                "5 MONEDAS = 1 TOKEN",
                400,
                279,
                12,
                LIGHTGRAY
            );

            drawRetroActionButton(
                {235.0f, 318.0f, 330.0f, 34.0f},
                "A",
                "REINICIAR",
                NEO_CYAN
            );

            drawRetroActionButton(
                {235.0f, 354.0f, 330.0f, 34.0f},
                "B",
                "SALIR AL MENU",
                WHITE
            );

            if (mostrarMensajesApi) {
                drawCyberText(
                    mensajeApi.c_str(),
                    215,
                    405,
                    15,
                    LIGHTGRAY
                );
            }

            break;
        }
    }

    if (
        screenTransitionAlpha > 0.0f &&
        shouldAnimateScreenTransition(currentScreen)
    ) {
        float easedAlpha =
                screenTransitionAlpha *
                screenTransitionAlpha *
                (3.0f - 2.0f * screenTransitionAlpha);
        unsigned char fadeAlpha =
                static_cast<unsigned char>(easedAlpha * 205.0f);

        DrawRectangle(
            0,
            0,
            screenWidth,
            screenHeight,
            {0, 0, 0, fadeAlpha}
        );
    }

    drawPopupModal();
}

void Game::drawPopupModal() {
    if (!popupActivo) {
        return;
    }

    DrawRectangle(0, 0, screenWidth, screenHeight, {0, 0, 0, 175});
    drawCyberPanel(150, 135, 500, 180, NEO_RED);

    drawCenteredCyberText(
        popupTitulo.c_str(),
        400,
        160,
        26,
        NEO_YELLOW
    );

    int messageWidth = MeasureText(popupMensaje.c_str(), 17);
    drawCyberText(
        popupMensaje.c_str(),
        400 - messageWidth / 2,
        210,
        17,
        WHITE
    );

    drawRetroRoundButton(315.0f, 270.0f, "A", NEO_CYAN);
    drawCyberText(
        popupBoton.c_str(),
        345,
        262,
        18,
        LIGHTGRAY
    );
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
