// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12Viewport.cpp - Stuff related to what we're outputting to monitors.
//==========================================================
#include "Renderer.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "D3D12Viewport.h"
#include <assert.h>

void D3D12Viewport::CreateSwapChain(HWND InWindowHandle)
{
	assert(ParentAdapter != nullptr);
	assert(ParentAdapter->RootDevice != nullptr);
	assert(ParentAdapter->RootDevice->d3dDevice != nullptr);

	WindowHandle = InWindowHandle;

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameBufferCount;
	swapChainDesc.Width = WinWidth;
	swapChainDesc.Height = WinHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullscreenDesc = {};
	swapChainFullscreenDesc.RefreshRate.Numerator = 0;
	swapChainFullscreenDesc.RefreshRate.Denominator = 0;
	swapChainFullscreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainFullscreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainFullscreenDesc.Windowed = !GIsFullscreen;

	IDXGISwapChain1* swapChain;
	ParentAdapter->DXGIFactory->CreateSwapChainForHwnd(
		ParentAdapter->RootDevice->CommandQueue,		// MULTIGPUTODO: for EACH DEVICE
		WindowHandle,
		&swapChainDesc,
		&swapChainFullscreenDesc,
		/*pRestrictToOutput=*/nullptr,
		&swapChain);

	ParentAdapter->DXGIFactory->MakeWindowAssociation(WindowHandle, DXGI_MWA_NO_ALT_ENTER);

	SwapChain = static_cast<IDXGISwapChain3*>(swapChain);
	Renderer::FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameBufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ParentAdapter->RootDevice->d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&FrameBufferRTVHeap)); // MULTIGPUTODO: for EACH DEVICE
		FrameBufferRTVHeap->SetName(L"SwapChainRTVHeap");
		FrameBufferRTVDescriptorSize = ParentAdapter->RootDevice->d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(FrameBufferRTVHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (uint32 n = 0; n < FrameBufferCount; n++)
		{
			SwapChain->GetBuffer(n, IID_PPV_ARGS(&FrameBuffers[n]));
			ParentAdapter->RootDevice->d3dDevice->CreateRenderTargetView(FrameBuffers[n], nullptr, rtvHandle); // MULTIGPUTODO: for EACH DEVICE
			FrameBuffers[n]->SetName(L"SwapChainRTV");
			rtvHandle.Offset(1, FrameBufferRTVDescriptorSize);
		}
	}

	// Create synchronization objects.
	{
		ParentAdapter->RootDevice->d3dDevice->CreateFence(/*InitialValue=*/0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&FrameFence)); // MULTIGPUTODO: for EACH DEVICE
		FrameFence->SetName(L"FrameFence");

		for (uint32 i = 0; i < FrameBufferCount; i++)
		{
			FrameFenceValues[i] = 1;
		}

		// Create an event handle to use for frame synchronization.
		FrameFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (FrameFenceEvent == nullptr)
		{
			HRESULT_FROM_WIN32(GetLastError());
		}
	}
}

void D3D12Viewport::WaitForGPUToFinish()
{
	// Schedule a Signal command in the queue.
	ParentAdapter->RootDevice->CommandQueue->Signal(FrameFence, FrameFenceValues[Renderer::FrameIndex]);

	// Wait until the fence has been processed.
	FrameFence->SetEventOnCompletion(FrameFenceValues[Renderer::FrameIndex], FrameFenceEvent);
	WaitForSingleObjectEx(FrameFenceEvent, INFINITE, FALSE);

	// Increment the fence value for the current frame.
	FrameFenceValues[Renderer::FrameIndex]++;
}

//turgle - move later
CD3DX12_VIEWPORT Viewport(0.0f, 0.0f, static_cast<float>(WinWidth), static_cast<float>(WinHeight));
CD3DX12_RECT ScissorRect(0, 0, static_cast<LONG>(WinWidth), static_cast<LONG>(WinHeight));
void D3D12Viewport::PrepareNextFrame() //turgle
{
	//-------
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	ParentAdapter->RootDevice->CommandAllocators[Renderer::FrameIndex]->Reset();

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	ParentAdapter->RootDevice->CommandList->Reset(ParentAdapter->RootDevice->CommandAllocators[Renderer::FrameIndex], ParentAdapter->GetPipelineState());

	// Set necessary state.
	ParentAdapter->RootDevice->CommandList->RSSetViewports(1, &Viewport);
	ParentAdapter->RootDevice->CommandList->RSSetScissorRects(1, &ScissorRect);

	// Indicate that the back buffer will be used as a render target.
	ParentAdapter->RootDevice->CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(FrameBuffers[Renderer::FrameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(FrameBufferRTVHeap->GetCPUDescriptorHandleForHeapStart(), Renderer::FrameIndex, FrameBufferRTVDescriptorSize);
	ParentAdapter->RootDevice->CommandList->OMSetRenderTargets(/*NumRenderTargetDescriptors=*/1, &rtvHandle, /*RTsSingleHandleToDescriptorRange=*/FALSE, /*pDepthStencilDescriptor*/nullptr);

	// Record commands.
	const float clearColor[] = { 0.2f, 0.0f, 0.4f, 1.0f };
	ParentAdapter->RootDevice->CommandList->ClearRenderTargetView(rtvHandle, clearColor, /*NumRects=*/ 0, /*pRects=*/nullptr);
}

void D3D12Viewport::Present()
{
	// Indicate that the back buffer will now be used to present.
	ParentAdapter->RootDevice->CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(FrameBuffers[Renderer::FrameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ParentAdapter->RootDevice->CommandList->Close();
	//-------
	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { ParentAdapter->RootDevice->CommandList };
	ParentAdapter->RootDevice->CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	//-------
	// Present the frame.
	SwapChain->Present(/*Syncinterval=*/1, /*Flags=*/0);

	const uint64 currentFenceValue = FrameFenceValues[Renderer::FrameIndex];

	// Schedule a Signal command in the queue.
	ParentAdapter->RootDevice->CommandQueue->Signal(FrameFence, currentFenceValue);

	// Update the frame index.
	Renderer::FrameIndex = SwapChain->GetCurrentBackBufferIndex();

	// If the next frame is not ready to be rendered yet, wait until it is ready.
	if (FrameFence->GetCompletedValue() < FrameFenceValues[Renderer::FrameIndex])
	{
		FrameFence->SetEventOnCompletion(FrameFenceValues[Renderer::FrameIndex], FrameFenceEvent);
		WaitForSingleObjectEx(FrameFenceEvent, INFINITE, FALSE);
	}

	// Set the fence value for the next frame.
	FrameFenceValues[Renderer::FrameIndex] = currentFenceValue + 1;
}

D3D12Viewport::~D3D12Viewport()
{
	WaitForGPUToFinish();
	CloseHandle(FrameFenceEvent);
}
