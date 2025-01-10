#include <utils/logger.hpp>
#include <utils/win32_helpers.hpp>

#include "dx_device.hpp"

using namespace Microsoft::WRL;

void EnableDebugLayer() {
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugInterface;
	FloppyBird::Utils::ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
#endif
}

#if defined(_DEBUG)
void SetupDebuggingForDxDevice(ComPtr<ID3D12Device2> d3d12Device2) {
	ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(d3d12Device2.As(&pInfoQueue))) {
		// Enable debug messages in debug mode.
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY severitiesToIgnore[] = {
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID messageIdsToIgnore[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		NewFilter.DenyList.NumSeverities = _countof(severitiesToIgnore);
		NewFilter.DenyList.pSeverityList = severitiesToIgnore;
		NewFilter.DenyList.NumIDs = _countof(messageIdsToIgnore);
		NewFilter.DenyList.pIDList = messageIdsToIgnore;

		FloppyBird::Utils::ThrowIfFailed(pInfoQueue->PushStorageFilter(&NewFilter));
	}
}
#endif

ComPtr<ID3D12Device2> FloppyBird::Graphics::DX::CreateDevice(ComPtr<IDXGIAdapter4> adapter) {
	EnableDebugLayer();

	ComPtr<ID3D12Device2> d3d12Device2;
	FloppyBird::Utils::ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3d12Device2)));
	
#if defined(_DEBUG)
	SetupDebuggingForDxDevice(d3d12Device2);
#endif

	return d3d12Device2;
}