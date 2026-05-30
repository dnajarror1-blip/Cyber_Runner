#ifndef GAME_API_CONFIG_H // Evita que este archivo se incluya más de una vez.
#define GAME_API_CONFIG_H // Define la protección del archivo.

#include <string> // Permite usar std::string para manejar textos.

namespace GameApiConfig { // Agrupa toda la configuración del juego en un solo lugar.

    // =========================
    // CONFIGURACIÓN DEL SERVIDOR
    // =========================

    const std::string BASE_URL = "http://52.55.14.96:3000"; 
    // Dirección del servidor en AWS donde está publicada la API.

    // =========================
    // CONFIGURACIÓN DEL JUEGO
    // =========================

    const std::string API_KEY = "A_G2_DUNNO_RUNNER_KEY_2026";
    // API KEY del grupo/juego. Sirve para que el servidor sepa qué grupo y sección se está conectando.
    
    const std::string CODIGO_JUEGO = "A_G2_DUNNO_RUNNER";
    // Código único del juego registrado en la base de datos.
   
    const std::string VERSION_JUEGO = "1.0.0"; 
    // Versión del juego. Se guarda en la partida para auditoría o control.

    // =========================
    // COSTO DE PARTIDA
    // =========================

    const int COSTO_PARTIDA = 3;
    // Cantidad de tokens que se le descuentan al usuario cuando inicia una partida.
    // Cada grupo puede cambiar este valor.
    // El backend actualmente acepta de 0 a 1000.

    // =========================
    // REGLAS DE PREMIOS
    // =========================

    const int SCORE_PREMIO_BAJO = 50; 
    // Puntaje mínimo para que el usuario gane el premio bajo.

    const int SCORE_PREMIO_ALTO = 100; 
    // Puntaje mínimo para que el usuario gane el premio alto.

    const int PREMIO_BAJO = 10; 
    // Tokens que gana el usuario si alcanza SCORE_PREMIO_BAJO.

    const int PREMIO_ALTO = 30; 
    // Tokens que gana el usuario si alcanza SCORE_PREMIO_ALTO.

    // =========================
    // REPORTE DE SCORE
    // =========================

    const int REPORTAR_CADA_PUNTOS = 50; 
    // Cada cuántos puntos se recomienda mandar el score al servidor.
    // Ejemplo: si vale 50, el juego puede reportar score en 50, 100, 150, etc.
}

#endif // Finaliza la protección del archivo.