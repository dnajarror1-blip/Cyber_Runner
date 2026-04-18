cmake_minimum_required(VERSION 4.2)
project(Cyber_Runner)

set(CMAKE_CXX_STANDARD 20)

find_package(raylib REQUIRED)

add_executable(Cyber_Runner src/main.cpp)

target_link_libraries(Cyber_Runner raylib)
