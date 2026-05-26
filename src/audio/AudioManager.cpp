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
    jumpSound = LoadSound("assets/music/jump.wav");

    doubleJumpSound = LoadSound("assets/music/salto2.wav");

    fallSound = LoadSound("assets/music/fall.wav");

    coinSound = LoadSound("assets/music/coin.wav");

    droneImpactSound = LoadSound("assets/music/dronimpact.wav");

    boxImpactSound = LoadSound("assets/music/boximpact.wav");

    shieldSound = LoadSound("assets/music/shieldactive.wav");

    gameOverSound = LoadSound("assets/music/gameover.wav");

    runningSound = LoadSound("assets/music/running.wav");

    SetSoundVolume(runningSound, 0.50f);
}

void AudioManager::unload()
{
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
    if (!IsSoundPlaying(gameOverSound))
    {
        PlaySound(gameOverSound);
    }
}

void AudioManager::startRunning()
{
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
    runningPlaying = false;

    StopSound(runningSound);
}