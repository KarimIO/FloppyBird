#include <fstream>
#include <directx/d3dx12.h>

#include <graphics/dx_context.hpp>
#include <utils/logger.hpp>
#include <utils/win32_helpers.hpp>
#include <d3dcompiler.h>

#include "dx_pipeline.hpp"

using namespace Microsoft::WRL;

void FloppyBird::Graphics::DX::Pipeline::Initialize(Context* context, const wchar_t* vertexShader, const wchar_t* fragmentShader) {
	auto device = context->GetDevice();
	
	ComPtr<ID3DBlob> vertexShaderBlob;
	FloppyBird::Utils::ThrowIfFailed(D3DReadFileToBlob(vertexShader, &vertexShaderBlob));

	ComPtr<ID3DBlob> pixelShaderBlob;
	FloppyBird::Utils::ThrowIfFailed(D3DReadFileToBlob(fragmentShader, &pixelShaderBlob));

	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
	if (FAILED(
			device->CheckFeatureSupport(
				D3D12_FEATURE_ROOT_SIGNATURE,
				&featureData,
				sizeof(featureData)
			)
		)
	) {
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}

	D3D12_DESCRIPTOR_RANGE1 ranges[1];
	ranges[0].BaseShaderRegister = 0;
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	ranges[0].NumDescriptors = 1;
	ranges[0].RegisterSpace = 0;
	ranges[0].OffsetInDescriptorsFromTableStart = 0;
	ranges[0].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE;

	CD3DX12_ROOT_PARAMETER1 rootParameters[1] = {};
	rootParameters[0].InitAsConstants(64, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
	rootSignatureDesc.Desc_1_1.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.Desc_1_1.NumParameters = 1;
	rootSignatureDesc.Desc_1_1.pParameters = rootParameters;
	rootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
	rootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
	ID3DBlob* signature;
	ID3DBlob* error;

	FloppyBird::Utils::ThrowIfFailed(D3D12SerializeVersionedRootSignature(&rootSignatureDesc,
		&signature, &error));
	FloppyBird::Utils::ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(),
		signature->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)));
	rootSignature->SetName(L"Hello Triangle Root Signature");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

	psoDesc.InputLayout = { nullptr, 0 };

	psoDesc.pRootSignature = rootSignature.Get();

	D3D12_SHADER_BYTECODE vsBytecode;
	vsBytecode.pShaderBytecode = vertexShaderBlob->GetBufferPointer();
	vsBytecode.BytecodeLength = vertexShaderBlob->GetBufferSize();
	psoDesc.VS = vsBytecode;

	D3D12_SHADER_BYTECODE psBytecode;
	psBytecode.pShaderBytecode = pixelShaderBlob->GetBufferPointer();
	psBytecode.BytecodeLength = pixelShaderBlob->GetBufferSize();
	psoDesc.PS = psBytecode;

	D3D12_RASTERIZER_DESC rasterDesc;
	rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterDesc.DepthClipEnable = TRUE;
	rasterDesc.MultisampleEnable = FALSE;
	rasterDesc.AntialiasedLineEnable = FALSE;
	rasterDesc.ForcedSampleCount = 0;
	rasterDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	psoDesc.RasterizerState = rasterDesc;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	D3D12_BLEND_DESC blendDesc;
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
	const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
		FALSE,
		FALSE,
		D3D12_BLEND_ONE,
		D3D12_BLEND_ZERO,
		D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,
		D3D12_BLEND_ZERO,
		D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL,
	};

	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
		blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
	}

	psoDesc.BlendState = blendDesc;

	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;

	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	FloppyBird::Utils::ThrowIfFailed(
		device->CreateGraphicsPipelineState(
			&psoDesc,
			IID_PPV_ARGS(&pipelineState)
		)
	);
}
