#include "core/Game.h"

#include "../api/ApiClient.h"
#include "../api/GameApiConfig.h"

#include "../auth/LoginManager.h"

#include <iostream>
#include <string>

int main()
{
    ApiClient api(
        GameApiConfig::BASE_URL,
        GameApiConfig::API_KEY,
        GameApiConfig::CODIGO_JUEGO
    );

    LoginManager login(api);

    Game game(api);

    std::string error;

    bool ok =
        login.iniciarSesion(
            "usuario",
            "password",
            error
        );

    if (ok)
    {
        std::cout << "LOGIN OK\n";

        game.setUsuario(
            login.getUsuarioActual()
        );
    }
    else
    {
        std::cout
            << error
            << std::endl;
    }

    game.run();

    return 0;
}