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
#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h"

class D3D12Device;
class D3D12Adapter
{
public:
	D3D12Adapter()
		: DXGIAdapter(nullptr),
		DXGIFactory(nullptr),
		RootDevice(nullptr),
		AdapterNodes{nullptr},
		RootSignature(nullptr),
		PipelineStateObject(nullptr)
	{}

	void Initialize();

	IDXGIAdapter4* DXGIAdapter;
	IDXGIFactory6* DXGIFactory;

	// Linked device adapters have 1 root device
	D3D12Device* RootDevice;

	// Each node is a real actual physical graphics card
	D3D12Device* AdapterNodes[8]; //turgle - 8 because this is the hellscape I've been living in

	//make this a vector later
	//D3D12Device* ChildDevice; // MULTIGPUTODO: for EACH DEVICE

	IDXGIFactory6* GetFactory() const { return DXGIFactory; }
	ID3D12PipelineState* GetPipelineState() const;
	ID3D12RootSignature* GetRootSignature() const;

	class D3D12RootSignature* RootSignature;
	class D3D12PipelineStateObject* PipelineStateObject;
};