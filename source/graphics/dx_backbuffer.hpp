#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace FloppyBird::Graphics::DX {
	Microsoft::WRL::ComPtr<IDXGISwapChain4> CreateSwapChain(
		HWND hWnd,
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue,
		uint32_t width, uint32_t height,
		uint32_t bufferCount
	);
}
