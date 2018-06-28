// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12Viewport.h - Stuff related to what we're outputting to monitors.
//==============================================================================================================
#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "Renderer.h"
#include <dxgi1_6.h>

class D3D12Adapter;
class D3D12Viewport
{
public:
	D3D12Viewport(D3D12Adapter* InAdapter)
		: ParentAdapter(InAdapter),
		WindowHandle(nullptr),
		SwapChain(nullptr),
		FrameFenceEvent(nullptr),
		FrameFence(nullptr),
		FrameFenceValues{0},
		FrameBuffers{nullptr},
		FrameBufferRTVHeap(nullptr),
		FrameBufferRTVDescriptorSize(0)
	{}

	~D3D12Viewport();

	void CreateSwapChain(HWND InWindowHandle);
	void WaitForGPUToFinish();
	void PrepareNextFrame(); //turgle
	void Present();

	D3D12Adapter* ParentAdapter;

	HWND WindowHandle;
	IDXGISwapChain3* SwapChain;
	HANDLE FrameFenceEvent;
	ID3D12Fence* FrameFence;
	uint64 FrameFenceValues[FrameBufferCount];
	ID3D12Resource* FrameBuffers[FrameBufferCount];
	ID3D12DescriptorHeap* FrameBufferRTVHeap;
	UINT FrameBufferRTVDescriptorSize;
};