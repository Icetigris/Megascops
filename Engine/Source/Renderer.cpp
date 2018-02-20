// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Renderer.cpp - D3D12 renderer that's going to get refactored away soon enough
//==========================================================
#include "Renderer.h"
#include "Log.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"

HWND WindowHandle; //used in swapchain


void Renderer::Create(SDL_Window* SDLWin)
{
	SDL_SysWMinfo windowinfo;
	SDL_VERSION(&windowinfo.version);
	SDL_GetWindowWMInfo(SDLWin, &windowinfo);
	WindowHandle = windowinfo.info.win.window;

	// might need like a factory or something for multigpu
	Adapter = new D3D12Adapter(*this);
	Adapter->Initialize(WindowHandle);
}

void Renderer::Render()
{
	Adapter->Present();
}

void Renderer::Destroy()
{
	// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
	// This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
	// sample illustrates how to use fences for efficient resource usage and to
	// maximize GPU utilization.

	// Signal and increment the fence value.
	const UINT64 fence = Adapter->FenceValue;
	Adapter->ChildDevice->CommandQueue->Signal(Adapter->FrameFence, fence);
	Adapter->FenceValue++;

	// Wait until the previous frame is finished.
	if (Adapter->FrameFence->GetCompletedValue() < fence)
	{
		Adapter->FrameFence->SetEventOnCompletion(fence, Adapter->FenceEvent);
		WaitForSingleObject(Adapter->FenceEvent, INFINITE);
	}

	Adapter->FrameIndex = Adapter->SwapChain->GetCurrentBackBufferIndex();

	delete Adapter;
}