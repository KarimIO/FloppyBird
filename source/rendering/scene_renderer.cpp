#include <directx/d3dx12.h>

#include <graphics/dx_pipeline.hpp>

#include "scene_renderer.hpp"

void FloppyBird::Rendering::SceneRenderer::Render(Graphics::DX::Context& context, Gameplay::Scene& scene) {
	BeginRenderingQuads(context);

	Vector2 birdSize{ 30.0f, 30.0f };

	const Gameplay::Bird& bird = scene.GetBird();
	RenderQuad(bird.position, birdSize);

	float top = 480.0f / 2.0f;
	float bottom = -top;

	const Gameplay::Obstacle* obstacles = scene.GetObstacles();
	for (size_t i = 0; i < Gameplay::MAX_OBSTACLES; ++i) {
		const Gameplay::Obstacle& obstacle = obstacles[i];

		float topMidY = (top + obstacle.topLedge) / 2.0f;
		float topHeight = top - obstacle.topLedge;
		float bottomMidY = (obstacle.lowerLedge + bottom) / 2.0f;
		float bottomHeight = obstacle.lowerLedge - bottom;

		RenderQuad(Vector2{ obstacle.x, topMidY }, Vector2{ Gameplay::obstacleWidth, topHeight });
		RenderQuad(Vector2{ obstacle.x, bottomMidY }, Vector2{ Gameplay::obstacleWidth, bottomHeight });
	}

	EndRenderingQuads(context);
}
