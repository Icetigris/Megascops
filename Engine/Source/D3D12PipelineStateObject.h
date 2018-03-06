// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12PipelineStateObject.h - D3D12 logical device. Represents 1 GPU.
//==============================================================================================================
#pragma once
#include "DependencyNode.h"
#include <d3d12.h>
#include "d3dx12.h"
#include "Renderer.h"
#include "D3D12RootSignature.h"

class D3D12Adapter;
// Dependencies: child devices, root signature
class D3D12PipelineStateObject : public DependencyNode
{
public:
	D3D12PipelineStateObject(D3D12Adapter& InAdapter, D3D12RootSignature& InRootSignature);

	void Initialize();

	D3D12Adapter& ParentAdapter;
	D3D12RootSignature& RootSignature;

	ID3D12PipelineState* d3dPipelineState;

	//shaders
};