cmake_minimum_required(VERSION 3.20)
project(Cyber_Runner)

set(CMAKE_CXX_STANDARD 20)

# Includes propios
include_directories(include)

add_executable(Cyber_Runner
        src/main.cpp
        src/core/Game.cpp
        src/entities/Player.cpp
        src/entities/Obstacle.cpp
        src/ui/HUD.cpp
)

#  Detectar sistema
if (WIN32)
    # Ruta de MSYS2 (ajústala si es diferente)
    set(RAYLIB_PATH "C:/msys64/mingw64")

    target_include_directories(Cyber_Runner PRIVATE ${RAYLIB_PATH}/include)
    target_link_directories(Cyber_Runner PRIVATE ${RAYLIB_PATH}/lib)

    target_link_libraries(Cyber_Runner
            raylib
            opengl32
            gdi32
            winmm
    )

elseif (UNIX)
    # Linux (Zorin OS)
    find_package(raylib REQUIRED)

    target_link_libraries(Cyber_Runner raylib)
endif()