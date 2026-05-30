#pragma once
#include "raylib.h"

// SECCION: HUD
// Dibuja informacion sobre gameplay y tokens sin modificar reglas del juego.
class HUD {
public:
    // FUNCION: Carga iconos usados por poderes del HUD.
    void loadAssets();
    // FUNCION: Libera iconos cargados por loadAssets().
    void unloadAssets();

    // FUNCION: Dibuja puntaje, velocidad, record y poderes activos durante gameplay.
    void drawGameHUD(
        float speed,
        int score,
        int highScore,
        bool nitroActive,
        bool hasShield,
        float nitroProgress,
        float shieldProgress
    );
    // FUNCION: Dibuja tokens disponibles en el menu principal.
    void drawMenuHUD(int creditos);

private:
    // Iconos mostrados solo cuando nitro o escudo estan activos.
    Texture2D nitroIconTexture {};
    Texture2D shieldIconTexture {};
};
