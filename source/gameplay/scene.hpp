#pragma once

#include "bird.hpp"
#include "obstacle.hpp"

namespace FloppyBird::Gameplay {
	const size_t MAX_OBSTACLES = 5;

	class Scene {
	public:
		void Initialize(FloppyBird::Graphics::DX::Context& context);
		void Update(double dt);
		const Bird& GetBird() const;
		const Obstacle* GetObstacles() const;
	protected:
		Bird bird;
		Obstacle obstacles[MAX_OBSTACLES];

		float leftMostObstacleX = 0.0f;
	};
}
