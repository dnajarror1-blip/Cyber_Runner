#pragma once

#include <string>
#include "../api/ApiClient.h"

class LoginManager {
private:
    ApiClient& api;
    UsuarioApi usuarioActual;
    bool autenticado;

public:
    explicit LoginManager(ApiClient& apiClient);

    bool iniciarSesion(
        const std::string& username,
        const std::string& password,
        std::string& mensajeError
    );

    bool estaAutenticado() const;
    UsuarioApi getUsuarioActual() const;

    void cerrarSesion();
};