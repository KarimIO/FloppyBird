#include <utils/win32_helpers.hpp>

#include "dx_adapter.hpp"

using namespace Microsoft::WRL;

ComPtr<IDXGIAdapter4> FloppyBird::Graphics::DX::GetAdapter(bool useWarp) {
	ComPtr<IDXGIFactory4> dxgiFactory;
	UINT createFactoryFlags = 0;
#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	FloppyBird::Utils::ThrowIfFailed(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)));

	ComPtr<IDXGIAdapter1> dxgiAdapter1;
	ComPtr<IDXGIAdapter4> dxgiAdapter4;

	if (useWarp) {
		FloppyBird::Utils::ThrowIfFailed(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1)));
		FloppyBird::Utils::ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
	}
	else {
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i) {
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually 
			// creating it. The adapter with the largest dedicated video memory
			// is favored.

			// Don't use software adapters
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0) {
				break;
			}

			if (
				SUCCEEDED(
					D3D12CreateDevice(
						dxgiAdapter1.Get(),
						D3D_FEATURE_LEVEL_12_1,
						__uuidof(ID3D12Device),
						nullptr
					)
				) &&
				dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory
			) {

				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				FloppyBird::Utils::ThrowIfFailed(dxgiAdapter1.As(&dxgiAdapter4));
			}
		}
	}

	return dxgiAdapter4;
}
