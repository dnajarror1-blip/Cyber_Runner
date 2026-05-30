#include "core/Game.h"

#include "../api/ApiClient.h"
#include "../api/GameApiConfig.h"

#include "../auth/LoginManager.h"

#include <iostream>
#include <string>

const bool MODO_PRUEBA_SIN_API = false;

int main()
{
    // SECCION: Inicio del programa
    // Crea el cliente que comunica el juego con el backend.
    // La configuracion vive en api/GameApiConfig.h, pero este archivo solo la usa.
    ApiClient api(
        GameApiConfig::BASE_URL,
        GameApiConfig::API_KEY,
        GameApiConfig::CODIGO_JUEGO
    );

    std::string errorConexion;

    // SECCION: Verificacion de API
    // Antes de abrir el juego se comprueba que el backend responda.
    // Si falla, el programa termina sin entrar al menu para evitar una sesion incompleta.
    if (!MODO_PRUEBA_SIN_API && !api.probarConexion(errorConexion)) {
        std::cout << "No se pudo conectar con la API." << std::endl;
        std::cout << "URL: " << GameApiConfig::BASE_URL << std::endl;
        std::cout << "Detalle: " << errorConexion << std::endl;
        return 0;
    }

    // SECCION: Login desde main
    // LoginManager centraliza la autenticacion y Game usa esa sesion durante el flujo del juego.
    LoginManager login(api);

    // SECCION: Flujo principal del juego
    // Game queda conectado al cliente API y al login para manejar menus, partida y datos del usuario.
    Game game(api, login);

    // SECCION: Modo local de prueba
    // Permite ejecutar el juego sin servidor cuando se activa manualmente la constante superior.
    // IMPORTANTE: No representa el flujo normal de presentacion con API.
    if (MODO_PRUEBA_SIN_API)
    {
        std::cout << "MODO PRUEBA SIN API ACTIVADO." << std::endl;
        game.setModoPruebaSinApi(true);

        UsuarioApi usuarioLocal;
        usuarioLocal.username = "Jugador local";
        usuarioLocal.saldoTokens = 0;

        game.setUsuario(usuarioLocal);
    }

    // FUNCION: Inicia el ciclo principal de Raylib y mantiene el juego activo.
    game.run();

    return 0;
}
