#include <graphics/dx_context.hpp>

#include "scene.hpp"

float GetRand(float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

float GetDiffBetweenObstacles() {
	return GetRand(60.0f, 180.0f);
}

float leftBorder = -(FloppyBird::Gameplay::obstacleWidth / 2.0f) - 320.0;

void FloppyBird::Gameplay::Scene::Initialize(FloppyBird::Graphics::DX::Context& context) {
	context.GetWindow()->SetOnKeyPressCallback(VK_SPACE, std::bind(&Bird::OnJump, &bird));

	float holeSize = 120.0f;
	float halfHoleSize = holeSize / 2.0f;
	float margin = 20.0f;

	float top = (480.0 / 2.0f) - halfHoleSize - margin;
	float bottom = -top;

	leftMostObstacleX = leftBorder;

	for (size_t index = 0; index < MAX_OBSTACLES; ++index) {
		FloppyBird::Gameplay::Obstacle& obstacle = obstacles[index];
		leftMostObstacleX += GetDiffBetweenObstacles();
		obstacle.x = leftMostObstacleX;

		float midLedge = GetRand(bottom, top);
		obstacle.topLedge = midLedge + halfHoleSize;
		obstacle.lowerLedge = midLedge - halfHoleSize;
	}
}

void FloppyBird::Gameplay::Scene::Update(double dt) {
	bird.Update(dt);

	float moveSpeed = 80.0f;
	for (FloppyBird::Gameplay::Obstacle& obstacle : obstacles) {
		if (obstacle.x < leftBorder) {
			leftMostObstacleX += GetDiffBetweenObstacles();
			obstacle.x = leftMostObstacleX;
		}
		else {
			obstacle.x -= moveSpeed * dt;
		}
	}
}

const FloppyBird::Gameplay::Bird& FloppyBird::Gameplay::Scene::GetBird() const {
	return bird;
}

const FloppyBird::Gameplay::Obstacle* FloppyBird::Gameplay::Scene::GetObstacles() const {
	return obstacles;
}
