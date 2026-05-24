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

    std::string errorConexion;

    if (!api.probarConexion(errorConexion)) {
        std::cout << "No se pudo conectar con la API." << std::endl;
        std::cout << "URL: " << GameApiConfig::BASE_URL << std::endl;
        std::cout << "Detalle: " << errorConexion << std::endl;
        return 0;
    }

    LoginManager login(api);

    Game game(api);

    std::string username;
    std::string password;
    std::string error;

    std::cout << "===== LOGIN =====" << std::endl;
    std::cout << "Usuario: ";
    std::cin >> username;

    std::cout << "Password: ";
    std::cin >> password;

    bool ok =
        login.iniciarSesion(
            username,
            password,
            error
        );

    if (!ok) {
        std::cout << "LOGIN ERROR: " << error << std::endl;
        std::cout << "No se puede abrir el juego sin sesion activa." << std::endl;
        return 0;
    }

    std::cout << "LOGIN OK" << std::endl;

    UsuarioApi usuario = login.getUsuarioActual();
    game.setUsuario(usuario);

    game.run();

    return 0;
}