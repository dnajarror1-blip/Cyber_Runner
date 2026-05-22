#include "ApiClient.h" // Incluye la definición de la clase ApiClient.

#include <cpr/cpr.h> // Librería para hacer peticiones HTTP.
#include <nlohmann/json.hpp> // Librería para trabajar con JSON.

#include <utility> // Permite usar std::move.

using json = nlohmann::json; // Alias para escribir json en lugar de nlohmann::json.

// =========================
// CONSTRUCTOR
// =========================

ApiClient::ApiClient(std::string baseUrl, std::string apiKey, std::string codigoJuego)
    : baseUrl(std::move(baseUrl)), // Guarda la URL base del servidor.
      apiKey(std::move(apiKey)), // Guarda la API KEY del juego.
      codigoJuego(std::move(codigoJuego)), // Guarda el código del juego.
      userToken("") {} // Inicializa el token vacío porque todavía no hay login.

// =========================
// FUNCIÓN AUXILIAR PARA VALIDAR RESPUESTAS
// =========================

static bool respuestaOk(const cpr::Response& response, json& data, std::string& mensajeError) {
    // Esta función revisa si la respuesta HTTP fue correcta.

    if (response.error) { // Verifica si hubo error de conexión con CPR.
        mensajeError = "Error CPR: " + response.error.message; // Guarda el mensaje del error.
        return false; // Retorna false porque falló la petición.
    }

    try { // Intenta convertir la respuesta del servidor a JSON.
        data = json::parse(response.text); // Convierte el texto recibido en JSON.
    } catch (...) { // Si la respuesta no era JSON válido.
        mensajeError = "Respuesta no es JSON valido. HTTP " +
                       std::to_string(response.status_code) +
                       " | Respuesta: " + response.text; // Guarda el error completo.
        return false; // Retorna false porque no pudo interpretar la respuesta.
    }

    if (response.status_code < 200 || response.status_code >= 300) {
        // Verifica si el código HTTP no fue exitoso.

        if (data.contains("message")) { // Si el servidor mandó un mensaje de error.
            mensajeError = data["message"].get<std::string>(); // Usa el mensaje del servidor.
        } else {
            mensajeError = "Error HTTP " + std::to_string(response.status_code) +
                           " | Respuesta: " + response.text; // Mensaje genérico.
        }

        return false; // Retorna false porque hubo error HTTP.
    }

    if (data.contains("ok") && !data["ok"].get<bool>()) {
        // Verifica si la API respondió ok:false.

        if (data.contains("message")) { // Si viene mensaje.
            mensajeError = data["message"].get<std::string>(); // Guarda el mensaje.
        } else {
            mensajeError = "La API respondio ok=false"; // Mensaje genérico.
        }

        return false; // Retorna false porque la API indicó error.
    }

    return true; // Si pasó todas las validaciones, la respuesta está correcta.
}

// =========================
// PROBAR CONEXIÓN
// =========================

bool ApiClient::probarConexion(std::string& mensajeError) {
    // Llama al endpoint /api/external/info para comprobar que la API key funciona.

    const auto response = cpr::Get( // Hace petición GET.
        cpr::Url{baseUrl + "/api/external/info"}, // URL del endpoint de prueba.
        cpr::Header{ // Encabezados HTTP.
            {"x-api-key", apiKey} // Envía la API KEY del juego.
        }
    );

    json data; // Aquí se guardará la respuesta JSON.

    return respuestaOk(response, data, mensajeError); // Valida la respuesta.
}

// =========================
// LOGIN DEL JUGADOR
// =========================

bool ApiClient::loginJugador(
    const std::string& username,
    const std::string& password,
    UsuarioApi& usuario,
    std::string& mensajeError
) {
    // Crea el cuerpo JSON para enviar usuario y contraseña.
    json body = {
        {"username", username}, // Usuario ingresado en el juego.
        {"password", password} // Contraseña ingresada en el juego.
    };

    const auto response = cpr::Post( // Hace petición POST.
        cpr::Url{baseUrl + "/api/external/auth/login"}, // Endpoint de login externo.
        cpr::Header{ // Encabezados HTTP.
            {"Content-Type", "application/json"}, // Indica que se envía JSON.
            {"x-api-key", apiKey} // Envía la API KEY del juego.
        },
        cpr::Body{body.dump()} // Convierte el JSON a texto y lo envía.
    );

    json data; // Aquí se guardará la respuesta JSON.

    if (!respuestaOk(response, data, mensajeError)) { // Valida si la respuesta fue correcta.
        return false; // Si hubo error, retorna false.
    }

    userToken = data.value("token", ""); // Guarda el token del usuario.

    if (userToken.empty()) { // Verifica si el servidor no devolvió token.
        mensajeError = "La API no devolvio token de jugador"; // Guarda error.
        return false; // Retorna false porque no hay sesión válida.
    }

    const auto usuarioJson = data["usuario"]; // Obtiene el objeto usuario del JSON.

    usuario.idUsuario = usuarioJson.value("id_usuario", 0); // Guarda el ID del usuario.
    usuario.username = usuarioJson.value("username", ""); // Guarda el username.
    usuario.correo = usuarioJson.value("correo", ""); // Guarda el correo.
    usuario.saldoTokens = usuarioJson.value("saldo_tokens", 0); // Guarda los tokens actuales.

    return true; // Login exitoso.
}

// =========================
// VERIFICAR SESIÓN
// =========================

bool ApiClient::tieneSesion() const {
    return !userToken.empty(); // Hay sesión si el token no está vacío.
}

// =========================
// CERRAR SESIÓN LOCAL
// =========================

void ApiClient::cerrarSesion() {
    userToken.clear(); // Borra el token guardado.
}

// =========================
// INICIAR PARTIDA
// =========================

bool ApiClient::iniciarPartida(
    PartidaApi& partida,
    std::string& mensajeError,
    const std::string& versionJuego,
    int costoTokens
) {
    if (userToken.empty()) { // Verifica que haya login.
        mensajeError = "No hay sesion de jugador. Primero inicia sesion."; // Guarda error.
        return false; // No permite iniciar partida sin token.
    }

    json body = { // Crea el JSON que se enviará al servidor.
        {"codigo_juego", codigoJuego}, // Código del juego.
        {"version_juego", versionJuego}, // Versión del juego.
        {"costo_tokens", costoTokens} // Costo que el juego quiere cobrar.
    };

    const auto response = cpr::Post( // Hace petición POST.
        cpr::Url{baseUrl + "/api/external/games/start"}, // Endpoint para iniciar partida.
        cpr::Header{ // Encabezados HTTP.
            {"Content-Type", "application/json"}, // Indica JSON.
            {"x-api-key", apiKey}, // Envía API KEY del juego.
            {"Authorization", "Bearer " + userToken} // Envía token del jugador.
        },
        cpr::Body{body.dump()} // Convierte JSON a texto.
    );

    json data; // Guarda respuesta.

    if (!respuestaOk(response, data, mensajeError)) { // Valida respuesta.
        return false; // Si falla, retorna false.
    }

    const auto partidaJson = data["partida"]; // Obtiene objeto partida del JSON.

    partida.idPartida = partidaJson.value("id_partida", -1LL); // Guarda ID de partida.
    partida.idJuego = partidaJson.value("id_juego", 0); // Guarda ID del juego.
    partida.codigoJuego = partidaJson.value("codigo_juego", ""); // Guarda código del juego.
    partida.nombreJuego = partidaJson.value("juego", ""); // Guarda nombre del juego.
    partida.username = partidaJson.value("username", ""); // Guarda usuario.
    partida.costoTokens = partidaJson.value("costo_tokens", costoTokens); // Guarda costo cobrado.
    partida.saldoAntes = partidaJson.value("saldo_antes", 0); // Guarda saldo antes.
    partida.saldoDespues = partidaJson.value("saldo_despues", 0); // Guarda saldo después.

    return true; // Partida iniciada correctamente.
}

// =========================
// REPORTAR SCORE
// =========================

bool ApiClient::reportarScore(
    long long idPartida,
    int score,
    int nivel,
    std::string& mensajeError
) {
    if (userToken.empty()) { // Verifica que haya sesión.
        mensajeError = "No hay sesion de jugador. Primero inicia sesion."; // Guarda error.
        return false; // No reporta sin sesión.
    }

    json body = { // Crea el cuerpo JSON.
        {"id_partida", idPartida}, // ID de partida.
        {"score", score}, // Score actual.
        {"nivel", nivel} // Nivel actual.
    };

    const auto response = cpr::Post( // Hace petición POST.
        cpr::Url{baseUrl + "/api/external/games/score"}, // Endpoint para score.
        cpr::Header{ // Headers.
            {"Content-Type", "application/json"}, // Indica JSON.
            {"x-api-key", apiKey}, // API KEY del juego.
            {"Authorization", "Bearer " + userToken} // Token del usuario.
        },
        cpr::Body{body.dump()} // Envía JSON como texto.
    );

    json data; // Respuesta JSON.

    return respuestaOk(response, data, mensajeError); // Retorna si fue exitoso.
}

// =========================
// FINALIZAR PARTIDA
// =========================

bool ApiClient::finalizarPartida(
    long long idPartida,
    int scoreFinal,
    int nivelFinal,
    const std::string& resultado,
    int duracionSeg,
    int tokensGanados,
    std::string& mensajeError
) {
    if (userToken.empty()) { // Verifica sesión.
        mensajeError = "No hay sesion de jugador. Primero inicia sesion."; // Error.
        return false; // No finaliza sin sesión.
    }

    json body = { // Crea JSON de finalización.
        {"id_partida", idPartida}, // ID partida.
        {"score_final", scoreFinal}, // Score final.
        {"nivel_final", nivelFinal}, // Nivel final.
        {"resultado", resultado}, // WIN o LOSE.
        {"duracion_seg", duracionSeg}, // Duración en segundos.
        {"tokens_ganados", tokensGanados} // Tokens que el juego decide entregar.
    };

    const auto response = cpr::Post( // Hace petición POST.
        cpr::Url{baseUrl + "/api/external/games/finish"}, // Endpoint finalizar.
        cpr::Header{ // Headers.
            {"Content-Type", "application/json"}, // Indica JSON.
            {"x-api-key", apiKey}, // API KEY del juego.
            {"Authorization", "Bearer " + userToken} // Token del jugador.
        },
        cpr::Body{body.dump()} // Envía JSON.
    );

    json data; // Respuesta JSON.

    return respuestaOk(response, data, mensajeError); // Valida y retorna resultado.
}

// =========================
// CONSULTAR RANKING
// =========================

bool ApiClient::consultarRanking(
    std::vector<RankingItem>& ranking,
    std::string& mensajeError
) {
    const auto response = cpr::Get( // Hace petición GET.
        cpr::Url{baseUrl + "/api/external/ranking/game/" + codigoJuego}, // Endpoint de ranking.
        cpr::Header{ // Headers.
            {"x-api-key", apiKey} // API KEY del juego.
        }
    );

    json data; // Respuesta JSON.

    if (!respuestaOk(response, data, mensajeError)) { // Valida respuesta.
        return false; // Si falla, retorna false.
    }

    ranking.clear(); // Limpia ranking anterior.

    for (const auto& item : data["ranking"]) { // Recorre la lista recibida.
        RankingItem rankingItem; // Crea una fila de ranking.

        rankingItem.username = item.value("username", ""); // Guarda username.
        rankingItem.bestScore = item.value("best_score", 0); // Guarda mejor score.
        rankingItem.bestNivel = item.value("best_nivel", 0); // Guarda mejor nivel.

        ranking.push_back(rankingItem); // Agrega la fila al vector.
    }

    return true; // Ranking consultado correctamente.
}