#include <array>

#include <directx/d3dx12_core.h>
#include <graphics/dx_pipeline.hpp>

#include "renderer.hpp"

struct QuadRenderData {
	float x;
	float y;
	float width;
	float height;
};

namespace FloppyBird::Rendering {
	const size_t MAX_DRAWABLES = 14;
	FloppyBird::Graphics::DX::Pipeline quadPipeline;

	struct QuadBuffer {
		std::array<QuadRenderData, MAX_DRAWABLES> renderData;
	} quadBuffer;

	size_t currentDrawCount = 0;
}

void FloppyBird::Rendering::Initialize(Graphics::DX::Context& context) {
	quadPipeline.Initialize(&context, L"../shaders/vertex.cso", L"../shaders/fragment.cso");
}

void FloppyBird::Rendering::BeginRenderingQuads(FloppyBird::Graphics::DX::Context& context) {
	FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };

	auto commandList = context.AcquireCommandList();
	context.ClearBackbuffer(commandList, clearColor);

	currentDrawCount = 0;

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	commandList->SetGraphicsRootSignature(quadPipeline.rootSignature.Get());
	commandList->SetPipelineState(quadPipeline.pipelineState.Get());

	uint32_t width;
	uint32_t height;
	context.GetScreenSize(width, height);

	D3D12_RECT scissorRect(CD3DX12_RECT(0, 0, LONG_MAX, LONG_MAX));
	D3D12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	const D3D12_CPU_DESCRIPTOR_HANDLE& rtv = context.GetCurrentRenderTargetView();
	commandList->OMSetRenderTargets(1, &rtv, FALSE, nullptr);

}

void FloppyBird::Rendering::RenderQuad(Vector2 position, Vector2 size) {
	QuadRenderData& quadData = quadBuffer.renderData[currentDrawCount++];
	quadData.x = position.x;
	quadData.y = position.y;
	quadData.width = size.x;
	quadData.height = size.y;
}

void FloppyBird::Rendering::EndRenderingQuads(FloppyBird::Graphics::DX::Context& context) {
	auto commandList = context.GetCurrentCommandList();

	commandList->SetGraphicsRoot32BitConstants(0, sizeof(QuadBuffer) / 4, &quadBuffer, 0);
	commandList->DrawInstanced(4, currentDrawCount, 0, 0);

	context.PresentCommandList(commandList);
}
