cmake_minimum_required(VERSION 3.20)
project(Cyber_Runner)

# SECCION: Configuracion C++
# El proyecto usa C++20 para compilar el juego y las clases auxiliares.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Librerías necesarias para la API
# SECCION: Dependencias API
# cpr se usa para HTTP y nlohmann_json para manejar JSON del backend.
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Includes propios
# SECCION: Includes del proyecto
# Permite incluir headers desde include/ usando rutas como core/Game.h.
include_directories(include)

# SECCION: Archivos fuente
# Aqui se listan los .cpp y .h que forman el ejecutable Cyber_Runner.
# IMPORTANTE: api/ aparece aqui solo para compilar; no modificar esos archivos.
add_executable(Cyber_Runner
        src/main.cpp
        src/core/Game.cpp
        src/entities/Player.cpp
        src/entities/Obstacle.cpp
        src/entities/Coin.cpp
        src/ui/HUD.cpp
        src/data/DataManager.cpp
        src/audio/AudioManager.cpp

        include/entities/Coin.h
        include/data/DataManager.h
        include/data/PlayerData.h
        include/audio/AudioManager.h

        api/ApiClient.cpp
        api/ApiClient.h
        api/GameApiConfig.h

        auth/LoginManager.cpp
        auth/LoginManager.h


)

# SECCION: Assets
# Copia la carpeta assets al directorio del ejecutable para que Raylib encuentre texturas y audio.
add_custom_command(TARGET Cyber_Runner POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:Cyber_Runner>/assets
)

# Detectar sistema
if (WIN32)
    # SECCION: Toolchain Windows
    # Usar MSYS2 UCRT64 para no mezclar toolchains
    set(MSYS2_PATH "C:/msys64/ucrt64")

    # Raylib se toma desde la instalacion UCRT64 configurada para CLion.
    target_include_directories(Cyber_Runner PRIVATE ${MSYS2_PATH}/include)
    target_link_directories(Cyber_Runner PRIVATE ${MSYS2_PATH}/lib)

    target_link_libraries(Cyber_Runner
            raylib
            opengl32
            gdi32
            winmm
            cpr::cpr
            nlohmann_json::nlohmann_json
    )

elseif (UNIX)
    # SECCION: Linux/Unix
    # En Unix se espera raylib disponible como paquete de CMake.
    find_package(raylib REQUIRED)

    target_link_libraries(Cyber_Runner
            raylib
            cpr::cpr
            nlohmann_json::nlohmann_json
    )
endif()