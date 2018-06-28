// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12VertexBuffer.cpp - Vertex buffers and vertex buffer views.
//==========================================================
#include "D3D12VertexBuffer.h"

void D3D12VertexBuffer::Initialize(ID3D12Device* d3dDevice, const uint32 vertexBufferSize, const uint32 vertexTypeSize, const void* InVertices)
{
	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	d3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&VertexBuffer));

	// Copy the vertex data to the vertex buffer.
	uint8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
	VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
	memcpy(pVertexDataBegin, InVertices, vertexBufferSize);
	VertexBuffer->Unmap(0, nullptr);

	// Initialize the vertex buffer view.
	VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
	VertexBufferView.StrideInBytes = vertexTypeSize;
	VertexBufferView.SizeInBytes = vertexBufferSize;
}