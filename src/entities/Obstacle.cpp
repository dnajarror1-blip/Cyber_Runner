//
// Created by darwin on 18/04/26.
//

#include <entities/Obstacle.h>

Obstacle::Obstacle(float x, float y, float width, float height, float speed) {
    rect = {x, y, width, height};
    this->speed = speed;
}

void Obstacle::update() {
    rect.x -= speed;

    //si sale de la pantalla reaparece
    if (rect.x + rect.width < 0) {
        rect.x = 800 + GetRandomValue(0, 200);
    }
}

void Obstacle::draw() {
    DrawRectangleRec(rect, {255, 0, 255,255}); //magenta neón

}

Rectangle Obstacle::getRect() {
    return rect;
}