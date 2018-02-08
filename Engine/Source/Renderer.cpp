// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Renderer.cpp - D3D12 renderer that's going to get refactored away soon enough
//==========================================================
#include "Renderer.h"
#include "Log.h"

#include <d3d12.h>
#include <dxgi1_4.h>

HWND WindowHandle; //used in swapchain

//a hot mess of things jacked from the d3d12 samples
static const UINT FrameCount = 2;

// Pipeline objects.
IDXGISwapChain3* m_swapChain;
ID3D12Device* m_device;
ID3D12Resource* m_renderTargets[FrameCount];
ID3D12CommandAllocator* m_commandAllocator;
ID3D12CommandQueue* m_commandQueue;
ID3D12DescriptorHeap* m_rtvHeap;
ID3D12PipelineState* m_pipelineState;
ID3D12GraphicsCommandList* m_commandList;
UINT m_rtvDescriptorSize;

// Synchronization objects.
UINT m_frameIndex;
HANDLE m_fenceEvent;
ID3D12Fence* m_fence;
UINT64 m_fenceValue;

//also copied from the d3d12 samples lol
// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_
void GetHardwareAdapter(IDXGIFactory2* pFactory, IDXGIAdapter1** ppAdapter)
{
	IDXGIAdapter1* adapter;
	*ppAdapter = nullptr;

	for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != pFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	*ppAdapter = adapter;
}

void CreateRenderer(SDL_Window* SDLWin)
{
	SDL_SysWMinfo windowinfo;
	SDL_VERSION(&windowinfo.version);
	SDL_GetWindowWMInfo(SDLWin, &windowinfo);
	WindowHandle = windowinfo.info.win.window;

	UINT dxgiFactoryFlags = 0;

#if 0// defined(_DEBUG)
	// Enable the debug layer (requires the Graphics Tools "optional feature").
	// NOTE: Enabling the debug layer after device creation will invalidate the active device.
	{
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	IDXGIFactory4* factory;
	(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	if (0)
	{
		IDXGIAdapter* warpAdapter;
		(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		D3D12CreateDevice(
			warpAdapter,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		);
	}
	else
	{
		IDXGIAdapter1* hardwareAdapter;
		GetHardwareAdapter(factory, &hardwareAdapter);

		D3D12CreateDevice(
			hardwareAdapter,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		);
	}

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = WinWidth;
	swapChainDesc.Height = WinHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	IDXGISwapChain1* swapChain;
	(factory->CreateSwapChainForHwnd(
		m_commandQueue,		// Swap chain needs the queue so that it can force a flush on it.
		WindowHandle,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	// This sample does not support fullscreen transitions.
	(factory->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_ALT_ENTER));

	//swapChain.As(&m_swapChain);
	m_swapChain = static_cast<IDXGISwapChain3*>(swapChain);
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
// 	{
// 		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
// 
// 		// Create a RTV for each frame.
// 		for (UINT n = 0; n < FrameCount; n++)
// 		{
// 			(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
// 			m_device->CreateRenderTargetView(m_renderTargets[n], nullptr, rtvHandle);
// 			rtvHandle.Offset(1, m_rtvDescriptorSize);
// 		}
// 	}

	m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
}

void Render()
{
}

void DestroyRenderer()
{
}