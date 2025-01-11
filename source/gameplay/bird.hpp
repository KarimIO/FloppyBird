#pragma once

#include <utils/vector2.hpp>

namespace FloppyBird::Gameplay {
    struct Bird {
        Vector2 position;
        Vector2 velocity;

        void OnJump();
        void Update(float dt);
    };
}
