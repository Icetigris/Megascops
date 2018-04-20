// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12Device.h - D3D12 logical device. Represents 1 GPU.
//==============================================================================================================
#pragma once
#include "DependencyNode.h"
#include <d3d12.h>
#include "d3dx12.h"
#include "Renderer.h"

// Dependencies: D3D12Adapter
class D3D12Adapter;
class D3D12Device : public DependencyNode
{
public:
	D3D12Device(D3D12Adapter& InAdapter);

	void Initialize();

	D3D12Adapter& GetParentAdapter() const { return ParentAdapter; }
	
	void Draw();

	D3D12Adapter& ParentAdapter;
	ID3D12Device* d3dDevice;
	ID3D12CommandAllocator* CommandAllocators[FrameBufferCount];
	ID3D12CommandQueue* CommandQueue; // can have at least 3 of these, one of each type (direct, compute, copy)
	ID3D12GraphicsCommandList* CommandList;

	//turgle temp move later
	class D3D12VertexBuffer* VertexBuffer;
	class D3D12ConstantBuffer* ConstantBuffer;
};