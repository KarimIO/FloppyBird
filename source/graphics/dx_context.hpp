#pragma once

#include <stdint.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include <windowing/window.hpp>

namespace FloppyBird::Graphics {
	const uint8_t FRAME_COUNT = 3;
}

namespace FloppyBird::Windowing {
	class Window;
}

namespace FloppyBird::Graphics::DX {
	class Context {
	public:
		bool Initialize(FloppyBird::Windowing::Window* window);
		~Context();
		void Flush();
		Microsoft::WRL::ComPtr<ID3D12Device2> GetDevice();
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> AcquireCommandList();
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetCurrentCommandList();
		void ClearBackbuffer(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList, FLOAT clearColor[]);
		void PresentCommandList(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);
		void OnResize(uint32_t width, uint32_t height);
		void GetScreenSize(uint32_t& width, uint32_t& height);
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRenderTargetView() const;
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentCbufferDescriptorSet() const;
		FloppyBird::Windowing::Window* GetWindow() const;
	protected:
		FloppyBird::Windowing::Window* window = nullptr;
		bool useWarp = false;
		uint32_t clientWidth = 2;
		uint32_t clientHeight = 2;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> adapter;
		Microsoft::WRL::ComPtr<ID3D12Device2> device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[FRAME_COUNT];
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[FRAME_COUNT];
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> renderTargetViewDescriptorHeap;
		UINT renderTargetViewDescriptorSize;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbufferDescriptorHeap;
		UINT cbufferDescriptorSize;
		UINT currentBackBufferIndex;

		// Synchronization objects
		Microsoft::WRL::ComPtr<ID3D12Fence> fence;
		uint64_t fenceValue = 0;
		uint64_t frameFenceValues[FRAME_COUNT] = {};
		HANDLE fenceEvent;
	};
}
