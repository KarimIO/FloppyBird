#include <directx/d3dx12.h>

#include <utils/win32_helpers.hpp>
#include <windowing/windowWin32.hpp>

#include "dx_adapter.hpp"
#include "dx_device.hpp"
#include "dx_backbuffer.hpp"

#include "dx_context.hpp"

using namespace Microsoft::WRL;

ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) {
	ComPtr<ID3D12CommandQueue> d3d12CommandQueue;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	FloppyBird::Utils::ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));

	return d3d12CommandQueue;
}


ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
	ComPtr<ID3D12Device2> device,
	D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t numDescriptors
) {
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = numDescriptors;
	desc.Type = type;

	FloppyBird::Utils::ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));

	return descriptorHeap;
}


ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(
	ComPtr<ID3D12Device2> device,
	D3D12_COMMAND_LIST_TYPE type
) {
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	FloppyBird::Utils::ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));

	return commandAllocator;

}



ComPtr<ID3D12GraphicsCommandList> CreateCommandList(
	ComPtr<ID3D12Device2> device,
	ComPtr<ID3D12CommandAllocator> commandAllocator,
	D3D12_COMMAND_LIST_TYPE type
) {
	ComPtr<ID3D12GraphicsCommandList> commandList;
	FloppyBird::Utils::ThrowIfFailed(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
	FloppyBird::Utils::ThrowIfFailed(commandList->Close());

	return commandList;
}



ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> device) {
	ComPtr<ID3D12Fence> fence;
	FloppyBird::Utils::ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

	return fence;
}

HANDLE CreateEventHandle() {
	HANDLE fenceEvent;

	fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (fenceEvent == 0) {
		F_LOG_FATAL(FloppyBird::Logger::LogSource::DirectX, "Failed to create fence event.");
	}

	return fenceEvent;
}


uint64_t SignalFence(
	ComPtr<ID3D12CommandQueue> commandQueue,
	ComPtr<ID3D12Fence> fence,
	uint64_t& fenceValue
) {
	uint64_t fenceValueForSignal = ++fenceValue;
	FloppyBird::Utils::ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValueForSignal));
	return fenceValueForSignal;
}

void WaitForFenceValue(
	ComPtr<ID3D12Fence> fence,
	uint64_t fenceValue, 
	HANDLE fenceEvent,
	std::chrono::milliseconds timeoutDuration = std::chrono::milliseconds::max()
) {
	if (fence->GetCompletedValue() < fenceValue)
	{
		FloppyBird::Utils::ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
		::WaitForSingleObject(fenceEvent, static_cast<DWORD>(timeoutDuration.count()));
	}
}

void UpdateRenderTargetViews(
	ComPtr<ID3D12Device2> device,
	ComPtr<IDXGISwapChain4> swapChain,
	ComPtr<ID3D12DescriptorHeap> descriptorHeap,
	ComPtr<ID3D12Resource>* backBuffers
) {
	auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < FloppyBird::Graphics::FRAME_COUNT; ++i) {
		ComPtr<ID3D12Resource> backBuffer;
		FloppyBird::Utils::ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

		device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

		backBuffers[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}
}

bool FloppyBird::Graphics::DX::Context::Initialize(FloppyBird::Windowing::Window* window) {
	adapter = FloppyBird::Graphics::DX::GetAdapter(false);
	if (adapter == nullptr) {
		return false;
	}

	device = FloppyBird::Graphics::DX::CreateDevice(adapter);
	if (device == nullptr) {
		return false;
	}

	this->window = window;
	commandQueue = CreateCommandQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT);

	window->SetOnResizeCallback(std::bind(&Context::OnResize, this, std::placeholders::_1, std::placeholders::_2));
	FloppyBird::Windowing::WindowWin32* windowWin32 = static_cast<FloppyBird::Windowing::WindowWin32*>(window);
	windowWin32->GetScreenSize(clientWidth, clientHeight);

	swapChain = CreateSwapChain(
		windowWin32->GetHandle(),
		commandQueue,
		clientWidth,
		clientHeight,
		FRAME_COUNT
	);

	currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();

	renderTargetViewDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FRAME_COUNT);
	renderTargetViewDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	cbufferDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, FRAME_COUNT);
	cbufferDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	UpdateRenderTargetViews(device, swapChain, renderTargetViewDescriptorHeap, backBuffers);

	for (int i = 0; i < FRAME_COUNT; ++i)
	{
		commandAllocators[i] = CreateCommandAllocator(device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	}

	commandList = CreateCommandList(
		device,
		commandAllocators[currentBackBufferIndex],
		D3D12_COMMAND_LIST_TYPE_DIRECT
	);

	fence = CreateFence(device);
	fenceEvent = CreateEventHandle();
}

FloppyBird::Graphics::DX::Context::~Context() {
	Flush();
	::CloseHandle(fenceEvent);
}

void FloppyBird::Graphics::DX::Context::Flush() {
	uint64_t fenceValueForSignal = SignalFence(commandQueue, fence, fenceValue);
	WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
}

Microsoft::WRL::ComPtr<ID3D12Device2> FloppyBird::Graphics::DX::Context::GetDevice() {
	return device;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> FloppyBird::Graphics::DX::Context::AcquireCommandList() {
	auto commandAllocator = commandAllocators[currentBackBufferIndex];
	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);

	return commandList;
}

Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> FloppyBird::Graphics::DX::Context::GetCurrentCommandList() {
	return commandList;
}

void FloppyBird::Graphics::DX::Context::ClearBackbuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, FLOAT clearColor[]) {
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers[currentBackBufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	commandList->ResourceBarrier(1, &barrier);
	D3D12_CPU_DESCRIPTOR_HANDLE rtv = GetCurrentRenderTargetView();

	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
}

void FloppyBird::Graphics::DX::Context::PresentCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList) {
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers[currentBackBufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrier);

	FloppyBird::Utils::ThrowIfFailed(commandList->Close());

	ID3D12CommandList* const commandLists[] = {
		commandList.Get()
	};
	commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	UINT syncInterval = 1;
	UINT presentFlags = 0;

	FloppyBird::Utils::ThrowIfFailed(swapChain->Present(syncInterval, presentFlags));

	frameFenceValues[currentBackBufferIndex] = SignalFence(commandQueue, fence, fenceValue);
	currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();

	WaitForFenceValue(fence, frameFenceValues[currentBackBufferIndex], fenceEvent);
}

void FloppyBird::Graphics::DX::Context::OnResize(uint32_t width, uint32_t height) {
	if (clientWidth != width || clientHeight != height) {
		// Don't allow 0 size swap chain back buffers.
		clientWidth = std::max(1u, width);
		clientHeight = std::max(1u, height);

		Flush();

		for (int i = 0; i < FRAME_COUNT; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			backBuffers[i].Reset();
			frameFenceValues[i] = frameFenceValues[currentBackBufferIndex];
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		FloppyBird::Utils::ThrowIfFailed(swapChain->GetDesc(&swapChainDesc));
		FloppyBird::Utils::ThrowIfFailed(swapChain->ResizeBuffers(
			FRAME_COUNT,
			clientWidth,
			clientHeight,
			swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

		currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();

		UpdateRenderTargetViews(device, swapChain, renderTargetViewDescriptorHeap, backBuffers);
	}
}

void FloppyBird::Graphics::DX::Context::GetScreenSize(uint32_t& width, uint32_t& height) {
	width = clientWidth;
	height = clientHeight;
}

D3D12_CPU_DESCRIPTOR_HANDLE FloppyBird::Graphics::DX::Context::GetCurrentRenderTargetView() const {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		renderTargetViewDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		currentBackBufferIndex,
		renderTargetViewDescriptorSize
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE FloppyBird::Graphics::DX::Context::GetCurrentCbufferDescriptorSet() const {
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		cbufferDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		currentBackBufferIndex,
		cbufferDescriptorSize
	);
}

FloppyBird::Windowing::Window* FloppyBird::Graphics::DX::Context::GetWindow() const {
	return window;
}
