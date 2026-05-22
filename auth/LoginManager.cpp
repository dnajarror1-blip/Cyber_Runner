#include "LoginManager.h"

LoginManager::LoginManager(ApiClient& apiClient)
    : api(apiClient),
      autenticado(false)
{
}

bool LoginManager::iniciarSesion(
    const std::string& username,
    const std::string& password,
    std::string& mensajeError
)
{
    if (username.empty())
    {
        mensajeError = "El usuario no puede estar vacio.";
        return false;
    }

    if (password.empty())
    {
        mensajeError = "La contrasena no puede estar vacia.";
        return false;
    }

    UsuarioApi usuario;

    bool loginOk = api.loginJugador(
        username,
        password,
        usuario,
        mensajeError
    );

    if (!loginOk)
    {
        autenticado = false;
        return false;
    }

    usuarioActual = usuario;
    autenticado = true;

    return true;
}

bool LoginManager::estaAutenticado() const
{
    return autenticado && api.tieneSesion();
}

UsuarioApi LoginManager::getUsuarioActual() const
{
    return usuarioActual;
}

void LoginManager::cerrarSesion()
{
    api.cerrarSesion();
    usuarioActual = UsuarioApi();
    autenticado = false;
}