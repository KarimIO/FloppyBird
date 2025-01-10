#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace FloppyBird::Graphics::DX {
	Microsoft::WRL::ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp);
}
