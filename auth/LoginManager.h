#pragma once

#include <string>
#include "../api/ApiClient.h"

// SECCION: Login desde main
// Administra la sesion del jugador sin mezclar la validacion de usuario con el flujo visual del juego.
class LoginManager {
private:
    // Referencia al cliente API ya creado en main.cpp.
    ApiClient& api;

    // Guarda los datos recibidos del servidor cuando el login es correcto.
    UsuarioApi usuarioActual;

    // Bandera local para saber si el login paso las validaciones de este manager.
    bool autenticado;

public:
    explicit LoginManager(ApiClient& apiClient);

    // FUNCION: Valida campos y solicita al backend iniciar sesion.
    bool iniciarSesion(
        const std::string& username,
        const std::string& password,
        std::string& mensajeError
    );

    // FUNCION: Confirma que exista login local y sesion activa en la API.
    bool estaAutenticado() const;

    // FUNCION: Devuelve el usuario que usa Game para nombre, saldo y datos de pantalla.
    UsuarioApi getUsuarioActual() const;

    // FUNCION: Cierra la sesion actual y limpia el usuario guardado.
    void cerrarSesion();
};