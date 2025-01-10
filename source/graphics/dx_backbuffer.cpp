#include <utils/logger.hpp>
#include <utils/win32_helpers.hpp>

#include "dx_backbuffer.hpp"

using namespace Microsoft::WRL;

Microsoft::WRL::ComPtr<IDXGISwapChain4> FloppyBird::Graphics::DX::CreateSwapChain(
	HWND hWnd,
	ComPtr<ID3D12CommandQueue> commandQueue,
	uint32_t width, uint32_t height,
	uint32_t bufferCount
) {
	ComPtr<IDXGISwapChain4> dxgiSwapChain4;
	ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	FloppyBird::Utils::ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)));
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = bufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0; // TODO: Check if we should use allow tearing?

	ComPtr<IDXGISwapChain1> swapChain1;
	FloppyBird::Utils::ThrowIfFailed(
		dxgiFactory4->CreateSwapChainForHwnd(
			commandQueue.Get(),
			hWnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1
		)
	);

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	FloppyBird::Utils::ThrowIfFailed(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));
	FloppyBird::Utils::ThrowIfFailed(swapChain1.As(&dxgiSwapChain4));

	return dxgiSwapChain4;
}