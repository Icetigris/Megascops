// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12Adapter.h - D3D12 hardware adapter. Can represent 1 or more devices.
//==============================================================================================================
/*
 * Single GPU case (the usual): 1 Adapter with 1 Device
 * Linked Device Adapter (LDA): 1 Adapter with 2+ nodes
 * Mixed Device Adapter (MDA): 2+ Adapters with 1 Device each
 *
 * LDA is usually 2 identical GPUs which are linked using SLI/Crossfire. 
 * They're guaranteed to have the same features and tiling formats, so you can directly send textures between cards.
 * You can make a shared swap chain so the driver can deal with syncing swap chain data instead of having to do it yourself.
 *
 * MDA is usually a discrete GPU and integrated graphics OR you have linked adapters but have SLI turned off.
 * You can't assume they share any tiling formats, so you're generally limited to row-major image layouts, which are only useful for copies really.
 *
 * Source: https://gpuopen.com/performance-tweets-series-multi-gpu/
*/
#pragma once
#include "DependencyNode.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include "d3dx12.h"

// Dependencies: the engine not crashing before this point I guess lol
class Renderer;
class D3D12Device;
// 2 for double buffering, 3 for triple buffering
static const uint32 FrameBufferCount = 2;
class D3D12Adapter : public DependencyNode
{
public:
	D3D12Adapter(Renderer& InRenderer)
		: DependencyNode(InRenderer, "D3D12Adapter")
	{
		PipelineState = nullptr;
		DXGIAdapter = nullptr;
	}

	void Initialize(HWND InWindowHandle);
	void CreateSwapChain(HWND InWindowHandle);
	void Present();

	IDXGIFactory4* GetFactory() const { return DXGIFactory; }
	ID3D12Resource* GetCurrentFrameBuffer() const { return FrameBuffers[FrameIndex]; }
	uint32 GetCurrentFrameBufferIndex() const { return FrameIndex; }

	IDXGIAdapter1* DXGIAdapter;
	IDXGIFactory4* DXGIFactory;
	ID3D12PipelineState* PipelineState;

	//make this a vector later
	D3D12Device* ChildDevice; // MULTIGPUTODO: for EACH DEVICE

	//uhhh moved this shit here from Display which I'm deleting
	HWND WindowHandle; //do I need to keep this around or not

	IDXGISwapChain3* SwapChain;
	uint32 FrameIndex;
	HANDLE FenceEvent;
	ID3D12Fence* FrameFence;
	uint64 FenceValue;
	ID3D12Resource* FrameBuffers[FrameBufferCount];
	ID3D12DescriptorHeap* RTVHeap;
	UINT RTVDescriptorSize;
};