#include "audio/AudioManager.h"

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
    unload();
}

void AudioManager::load()
{
    // SECCION: Audio - carga de assets
    // Las rutas apuntan a assets/music y se cargan una vez al iniciar Game::run().
    jumpSound = LoadSound("assets/music/jump.wav");

    doubleJumpSound = LoadSound("assets/music/salto2.wav");

    fallSound = LoadSound("assets/music/fall.wav");

    coinSound = LoadSound("assets/music/coin.wav");

    droneImpactSound = LoadSound("assets/music/dronimpact.wav");

    boxImpactSound = LoadSound("assets/music/boximpact.wav");

    shieldSound = LoadSound("assets/music/shieldactive.wav");

    gameOverSound = LoadSound("assets/music/gameover.wav");

    runningSound = LoadSound("assets/music/running.wav");

    // El loop de carrera se deja a menor volumen para no tapar efectos importantes.
    SetSoundVolume(runningSound, 0.50f);
}

void AudioManager::unload()
{
    // Detiene el loop antes de liberar sonidos.
    StopSound(runningSound);

    UnloadSound(jumpSound);

    UnloadSound(doubleJumpSound);

    UnloadSound(fallSound);

    UnloadSound(coinSound);

    UnloadSound(droneImpactSound);

    UnloadSound(boxImpactSound);

    UnloadSound(shieldSound);

    UnloadSound(gameOverSound);

    UnloadSound(runningSound);
}

void AudioManager::update()
{
    // SECCION: Loop de carrera
    // Si el jugador esta corriendo y el sonido termino, se vuelve a reproducir.
    if (runningPlaying)
    {
        if (!IsSoundPlaying(runningSound))
        {
            PlaySound(runningSound);
        }
    }
}

void AudioManager::playJump()
{
    PlaySound(jumpSound);
}

void AudioManager::playDoubleJump()
{
    PlaySound(doubleJumpSound);
}

void AudioManager::playLanding()
{
    PlaySound(fallSound);
}

void AudioManager::playCoin()
{
    PlaySound(coinSound);
}

void AudioManager::playDroneImpact()
{
    PlaySound(droneImpactSound);
}

void AudioManager::playBoxImpact()
{
    PlaySound(boxImpactSound);
}

void AudioManager::playShield()
{
    PlaySound(shieldSound);
}

void AudioManager::playGameOver()
{
    // Evita superponer varias veces el sonido de game over.
    if (!IsSoundPlaying(gameOverSound))
    {
        PlaySound(gameOverSound);
    }
}

void AudioManager::startRunning()
{
    // Solo inicia el loop si no estaba marcado como activo.
    if (!runningPlaying)
    {
        runningPlaying = true;

        if (!IsSoundPlaying(runningSound))
        {
            PlaySound(runningSound);
        }
    }
}

void AudioManager::stopRunning()
{
    // Se llama al saltar, pausar, impactar o salir de gameplay.
    runningPlaying = false;

    StopSound(runningSound);
}