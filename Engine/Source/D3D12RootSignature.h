// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12RootSignature.h - Defines which resources the shader needs bound to the pipeline.
//==============================================================================================================
#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "Renderer.h"

class D3D12RootSignature
{
public:
	D3D12RootSignature() {}

	void Initialize(ID3D12Device* d3dDevice, uint32 NodeMask);

	ID3D12RootSignature* d3dRootSignature;
};