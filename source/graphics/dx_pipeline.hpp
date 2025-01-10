#pragma once

#include <d3d12.h>
#include <wrl.h>

namespace FloppyBird::Graphics::DX {
	class Context;

	class Pipeline {
	public:
		void Initialize(Context* context, const wchar_t* vertexShader, const wchar_t* fragmentShader);

		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};
}
