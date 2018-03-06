// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12Adapter.cpp - D3D12 hardware adapter. Can represent 1 or more devices.
//==========================================================
#include "Renderer.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "Log.h"

//also copied from the d3d12 samples lol
// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
void D3D12Adapter::Initialize(HWND InWindowHandle) //d3ddebug or not?
{
	IDXGIAdapter1* adapter;

	uint32 dxgiFactoryFlags = 0;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&DXGIFactory));
#if (_DEBUG) //turgle - take a command  line arg to enable d3ddebug
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

	for (uint32 adapterIndex = 0; DXGI_ERROR_NOT_FOUND != DXGIFactory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}

	if (!adapter)
	{
		MEGALOGLN("Could not find a hardware adapter that supports D3D12.");
	}

	DXGIAdapter = adapter;

	// might need a factory for devices too
	ChildDevice = new D3D12Device(*this); //turgle this is messy and I don't like it but I want this to run before I unfuck it

	ChildDevice->Initialize(); // MULTIGPUTODO: for EACH DEVICE
	CreateSwapChain(InWindowHandle);
}

void D3D12Adapter::CreateSwapChain(HWND InWindowHandle)
{
	WindowHandle = InWindowHandle; //turgle - check handle integrity?

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameBufferCount;
	swapChainDesc.Width = WinWidth;
	swapChainDesc.Height = WinHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	IDXGISwapChain1* swapChain;
	DXGIFactory->CreateSwapChainForHwnd(
		ChildDevice->CommandQueue,		// MULTIGPUTODO: for EACH DEVICE
		WindowHandle,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain);

	DXGIFactory->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_ALT_ENTER);

	//swapChain.As(&m_swapChain);
	SwapChain = static_cast<IDXGISwapChain3*>(swapChain);
	Renderer::FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ChildDevice->D3DDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&RTVHeap)); // MULTIGPUTODO: for EACH DEVICE
		RTVHeap->SetName(L"SwapChainRTVHeap");
		RTVDescriptorSize = ChildDevice->D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(RTVHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (uint32 n = 0; n < FrameBufferCount; n++)
		{
			SwapChain->GetBuffer(n, IID_PPV_ARGS(&FrameBuffers[n]));
			ChildDevice->D3DDevice->CreateRenderTargetView(FrameBuffers[n], nullptr, rtvHandle); // MULTIGPUTODO: for EACH DEVICE
			FrameBuffers[n]->SetName(L"SwapChainRTV");
			rtvHandle.Offset(1, RTVDescriptorSize);
		}
	}

	// Create synchronization objects.
	{
		ChildDevice->D3DDevice->CreateFence(/*InitialValue=*/0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&FrameFence)); // MULTIGPUTODO: for EACH DEVICE
		FrameFence->SetName(L"FrameFence");

		for (uint32 i = 0; i < FrameBufferCount; i++)
		{
			FenceValues[i] = 1;
		}

		// Create an event handle to use for frame synchronization.
		FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (FenceEvent == nullptr)
		{
			HRESULT_FROM_WIN32(GetLastError());
		}
	}
}

void D3D12Adapter::WaitForGPUToFinish()
{
	// Schedule a Signal command in the queue.
	ChildDevice->CommandQueue->Signal(FrameFence, FenceValues[Renderer::FrameIndex]);

	// Wait until the fence has been processed.
	FrameFence->SetEventOnCompletion(FenceValues[Renderer::FrameIndex], FenceEvent);
	WaitForSingleObjectEx(FenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	FenceValues[Renderer::FrameIndex]++;
}

void D3D12Adapter::Present()
{
	ChildDevice->Draw();  // MULTIGPUTODO: for EACH DEVICE

	// Present the frame.
	SwapChain->Present(1, 0);

	const UINT64 currentFenceValue = FenceValues[Renderer::FrameIndex];

	// Schedule a Signal command in the queue.
	ChildDevice->CommandQueue->Signal(FrameFence, currentFenceValue);

	// Update the frame index.
	Renderer::FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (FrameFence->GetCompletedValue() < FenceValues[Renderer::FrameIndex])
	{
		FrameFence->SetEventOnCompletion(FenceValues[Renderer::FrameIndex], FenceEvent);
		WaitForSingleObjectEx(FenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	FenceValues[Renderer::FrameIndex] = currentFenceValue + 1;
}


D3D12Adapter::~D3D12Adapter()
{
	WaitForGPUToFinish();
	CloseHandle(FenceEvent);
}
