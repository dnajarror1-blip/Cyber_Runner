#include "raylib.h"

class Obstacle {
private:
    Rectangle rect;
    float speed;

public:
    // El constructor que ya tenías
    Obstacle(float x, float y, float width, float height, float speed);

    // Actualiza la posición y cambia el tipo de obstáculo al reaparecer
    void update(float deltaTime);

    // Dibuja el obstáculo (mientras no tengas los sprites finales)
    void draw();

    // FUNCIÓN NUEVA: Permite que la velocidad suba con el tiempo
    void setSpeed(float newSpeed);

    void forceRespawn();

    // Devuelve el rectángulo para detectar choques
    Rectangle getRect();
};


