// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12PipelineStateObject.h - Represents all the pipeline state, from Input Assembler to OutputMerger, for a draw call.
//==============================================================================================================
#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "Renderer.h"
#include "D3D12RootSignature.h"

class D3D12Adapter;
// Dependencies: child devices, root signature, shaders
class D3D12PipelineStateObject
{
public:
	D3D12PipelineStateObject() {}

	void Initialize(ID3D12Device* d3dDevice, ID3D12RootSignature* d3dRootSignature, uint32 NodeMask);
	
	ID3D12PipelineState* d3dPipelineState;

	//shaders
};