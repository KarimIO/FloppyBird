#pragma once

#include <graphics/dx_context.hpp>
#include <graphics/dx_pipeline.hpp>
#include <utils/vector2.hpp>

namespace FloppyBird::Rendering {
	void Initialize(FloppyBird::Graphics::DX::Context& context);
	void BeginRenderingQuads(FloppyBird::Graphics::DX::Context& context);
	void RenderQuad(Vector2 position, Vector2 size);
	void EndRenderingQuads(FloppyBird::Graphics::DX::Context& context);
}
