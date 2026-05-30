#pragma once

#include "raylib.h"

// SECCION: Audio
// Centraliza sonidos cortos del gameplay para que Game no cargue archivos de audio directamente.
class AudioManager
{
private:

    // Sonidos de movimiento del jugador.
    Sound jumpSound {};
    Sound doubleJumpSound {};
    Sound fallSound {};

    // Sonido de recoleccion.
    Sound coinSound {};

    // Sonidos de impacto separados por tipo de obstaculo.
    Sound droneImpactSound {};
    Sound boxImpactSound {};

    // Sonidos de poderes y fin de partida.
    Sound shieldSound {};

    Sound gameOverSound {};

    Sound runningSound {};

    // Controla si el sonido de carrera debe mantenerse en loop.
    bool runningPlaying = false;

public:

    AudioManager();

    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    // FUNCION: Carga todos los sonidos despues de InitAudioDevice().
    void load();

    // FUNCION: Libera todos los sonidos antes de CloseAudioDevice().
    void unload();

    // FUNCION: Mantiene activo el sonido de correr cuando corresponde.
    void update();

    // FUNCIONES: Reproducen eventos puntuales del gameplay.
    void playJump();

    void playDoubleJump();

    void playLanding();

    void playCoin();

    void playDroneImpact();

    void playBoxImpact();

    void playShield();

    void playGameOver();

    // FUNCION: Inicia o detiene el loop de carrera del jugador.
    void startRunning();

    void stopRunning();
};