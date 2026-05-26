#pragma once

#include "raylib.h"

class AudioManager
{
private:

    Sound jumpSound {};
    Sound doubleJumpSound {};
    Sound fallSound {};

    Sound coinSound {};

    Sound droneImpactSound {};
    Sound boxImpactSound {};

    Sound shieldSound {};

    Sound gameOverSound {};

    Sound runningSound {};

    bool runningPlaying = false;

public:

    AudioManager();

    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void load();

    void unload();

    void update();

    void playJump();

    void playDoubleJump();

    void playLanding();

    void playCoin();

    void playDroneImpact();

    void playBoxImpact();

    void playShield();

    void playGameOver();

    void startRunning();

    void stopRunning();
};