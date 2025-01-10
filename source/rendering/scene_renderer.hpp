#pragma once

#include <gameplay/scene.hpp>

#include "renderer.hpp"

namespace FloppyBird::Rendering {
	struct SceneRenderer {
		void Render(Graphics::DX::Context& context, Gameplay::Scene& scene);
	};
}
