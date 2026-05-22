#ifndef API_CLIENT_H // Evita que el archivo se incluya más de una vez.
#define API_CLIENT_H // Define la protección del archivo.

#include <string> // Permite usar std::string.
#include <vector> // Permite usar std::vector para listas, como el ranking.

// =========================
// ESTRUCTURA DEL USUARIO
// =========================

struct UsuarioApi { // Guarda la información del usuario que viene desde la API.
    int idUsuario = 0; // ID interno del usuario en la base de datos.
    std::string username; // Nombre de usuario.
    std::string correo; // Correo institucional del usuario.
    int saldoTokens = 0; // Cantidad de tokens actuales del usuario.
};

// =========================
// ESTRUCTURA DE LA PARTIDA
// =========================

struct PartidaApi { // Guarda la información de la partida creada por la API.
    long long idPartida = -1; // ID de la partida creada en SQL Server.
    int idJuego = 0; // ID interno del juego en la base de datos.
    std::string codigoJuego; // Código del juego, por ejemplo D_G2_SNAKE.
    std::string nombreJuego; // Nombre visible del juego, por ejemplo Snake.
    std::string username; // Usuario que inició la partida.
    int costoTokens = 0; // Tokens cobrados al iniciar la partida.
    int saldoAntes = 0; // Saldo que tenía el usuario antes de iniciar.
    int saldoDespues = 0; // Saldo que quedó después de descontar el costo.
};

// =========================
// ESTRUCTURA DEL RANKING
// =========================

struct RankingItem { // Guarda una fila del ranking.
    std::string username; // Nombre del jugador.
    int bestScore = 0; // Mejor score registrado.
    int bestNivel = 0; // Mejor nivel registrado.
};

// =========================
// CLASE PRINCIPAL API CLIENT
// =========================

class ApiClient { // Clase encargada de conectarse con la API del servidor.
private:
    std::string baseUrl; // Dirección base del servidor API.
    std::string apiKey; // API KEY del juego.
    std::string codigoJuego; // Código del juego.
    std::string userToken; // Token del usuario después de iniciar sesión.

public:
    ApiClient(std::string baseUrl, std::string apiKey, std::string codigoJuego);
    // Constructor: recibe la URL del servidor, la API KEY y el código del juego.

    bool probarConexion(std::string& mensajeError);
    // Prueba si la API externa responde correctamente.

    bool loginJugador(
        const std::string& username,
        const std::string& password,
        UsuarioApi& usuario,
        std::string& mensajeError
    );
    // Inicia sesión con usuario y contraseña.
    // Si funciona, guarda los datos del usuario y el token.

    bool tieneSesion() const;
    // Devuelve true si el usuario ya inició sesión y tiene token.

    void cerrarSesion();
    // Borra el token local del usuario.

    bool iniciarPartida(
        PartidaApi& partida,
        std::string& mensajeError,
        const std::string& versionJuego = "1.0.0",
        int costoTokens = 10
    );
    // Crea una partida en el servidor.
    // También descuenta los tokens indicados en costoTokens.

    bool reportarScore(
        long long idPartida,
        int score,
        int nivel,
        std::string& mensajeError
    );
    // Reporta el score actual de la partida.

    bool finalizarPartida(
        long long idPartida,
        int scoreFinal,
        int nivelFinal,
        const std::string& resultado,
        int duracionSeg,
        int tokensGanados,
        std::string& mensajeError
    );
    // Finaliza la partida en el servidor.
    // Guarda score final, nivel, resultado, duración y tokens ganados.

    bool consultarRanking(
        std::vector<RankingItem>& ranking,
        std::string& mensajeError
    );
    // Consulta el ranking del juego actual.
};

#endif // Finaliza la protección del archivo.