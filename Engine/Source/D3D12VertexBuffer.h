// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12VertexBuffer.h - Vertex buffers and vertex buffer views.
//==============================================================================================================
#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "Renderer.h"

// Dependencies: D3D12Device
class D3D12Device;
class D3D12VertexBuffer
{
public:
	D3D12VertexBuffer() {}

	void Initialize(ID3D12Device* d3dDevice);
	
	ID3D12Resource* VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
};