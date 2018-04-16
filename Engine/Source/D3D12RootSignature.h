// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12RootSignature.h - Defines which resources the shader needs bound to the pipeline.
//==============================================================================================================
#pragma once
#include "DependencyNode.h"
#include <d3d12.h>
#include "d3dx12.h"
#include "Renderer.h"

// Dependencies: child devices
class D3D12Adapter;
class D3D12RootSignature : public DependencyNode
{
public:
	D3D12RootSignature(D3D12Adapter& InAdapter);

	void Initialize();

	D3D12Adapter& ParentAdapter;
	ID3D12RootSignature* d3dRootSignature;
};