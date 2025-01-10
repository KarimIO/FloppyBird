#pragma once

namespace FloppyBird::Gameplay {
	const float obstacleWidth = 30.0f;

	struct Obstacle {
		float x;
		float topLedge;
		float lowerLedge;
	};
}
