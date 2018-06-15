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
	D3D12VertexBuffer(D3D12Device& InDevice);

	void Initialize();
	
	ID3D12Device* d3dDevice; //probably don't need this? should be able to just pass it into Initialize() and check if the device is ready?
	ID3D12Resource* VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
};