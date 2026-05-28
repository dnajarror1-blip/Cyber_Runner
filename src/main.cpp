#include "core/Game.h"

#include "../api/ApiClient.h"
#include "../api/GameApiConfig.h"

#include "../auth/LoginManager.h"

#include <iostream>
#include <string>

const bool MODO_PRUEBA_SIN_API = false;

int main()
{
    ApiClient api(
        GameApiConfig::BASE_URL,
        GameApiConfig::API_KEY,
        GameApiConfig::CODIGO_JUEGO
    );

    std::string errorConexion;

    if (!MODO_PRUEBA_SIN_API && !api.probarConexion(errorConexion)) {
        std::cout << "No se pudo conectar con la API." << std::endl;
        std::cout << "URL: " << GameApiConfig::BASE_URL << std::endl;
        std::cout << "Detalle: " << errorConexion << std::endl;
        return 0;
    }

    LoginManager login(api);

    Game game(api, login);

    if (MODO_PRUEBA_SIN_API)
    {
        std::cout << "MODO PRUEBA SIN API ACTIVADO." << std::endl;

        UsuarioApi usuarioLocal;
        usuarioLocal.username = "Jugador local";
        usuarioLocal.saldoTokens = 0;

        game.setUsuario(usuarioLocal);
    }

    game.run();

    return 0;
}
