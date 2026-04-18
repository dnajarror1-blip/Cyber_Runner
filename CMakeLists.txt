cmake_minimum_required(VERSION 4.2)
project(Cyber_Runner)

set(CMAKE_CXX_STANDARD 20)

find_package(raylib REQUIRED)

add_executable(Cyber_Runner src/main.cpp
        src/core/Game.cpp
        src/entities/Player.cpp
        src/entities/Obstacle.cpp
        src/ui/HUD.cpp
        include/core/Game.h
        include/entities/Player.h
        include/ui/HUD.h
        include/entities/Obstacle.h)

target_link_libraries(Cyber_Runner raylib)
