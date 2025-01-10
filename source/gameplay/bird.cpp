#include "bird.hpp"

void FloppyBird::Gameplay::Bird::OnJump() {
    velocity.y = 200.0;
}

void FloppyBird::Gameplay::Bird::Update(double dt) {
    velocity.y -= dt * 400.0f;
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    if (position.y > 300.0f) {
        position.y = -299.0f;
    }

    if (position.y < -300.0f) {
        position.y = 299.0f;
    }
}
