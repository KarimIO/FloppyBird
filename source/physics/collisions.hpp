#pragma once

namespace FloppyBird::Physics {
    struct CircleCollider {
        float centerX;
        float centerY;
        float radius;
    };
    
    struct BoxCollider {
        float x;
        float y;
        float width;
        float height;
    };
}
