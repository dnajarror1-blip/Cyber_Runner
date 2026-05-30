#include "LoginManager.h"

LoginManager::LoginManager(ApiClient& apiClient)
    : api(apiClient),
      autenticado(false)
{
}

// FUNCION: Intenta iniciar sesion con usuario y contrasena.
// Primero valida campos vacios y luego delega la autenticacion real al backend.
bool LoginManager::iniciarSesion(
    const std::string& username,
    const std::string& password,
    std::string& mensajeError
)
{
    // IMPORTANTE: Estas validaciones evitan llamar a la API con datos incompletos.
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

    // SECCION: Relacion con API
    // La API devuelve los datos del jugador que despues usara Game.
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

    // Si el backend acepta el login, se conserva el usuario para el resto del juego.
    usuarioActual = usuario;
    autenticado = true;

    return true;
}

// FUNCION: Comprueba el estado de autenticacion usado por pantallas y flujo de partida.
bool LoginManager::estaAutenticado() const
{
    return autenticado && api.tieneSesion();
}

// FUNCION: Entrega una copia de los datos actuales del jugador.
UsuarioApi LoginManager::getUsuarioActual() const
{
    return usuarioActual;
}

// FUNCION: Sale de la sesion y deja el manager listo para un nuevo login.
void LoginManager::cerrarSesion()
{
    api.cerrarSesion();
    usuarioActual = UsuarioApi();
    autenticado = false;
}