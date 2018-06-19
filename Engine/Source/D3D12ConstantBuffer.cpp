// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12ConstantBuffer.cpp - Constant buffers and constant buffer views.
//==========================================================
#include "D3D12ConstantBuffer.h"

void D3D12ConstantBuffer::Initialize(ID3D12Device* d3dDevice, uint32 NodeMask, uint32 ConstantBufferSize, void* InConstantBuffer)
{
	// Describe and create a constant buffer view (CBV) descriptor heap.
	// Flags indicate that this descriptor heap can be bound to the pipeline 
	// and that descriptors contained in it can be referenced by a root table.
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = NodeMask;
	d3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&CBVHeap));

	// Create the constant buffer.
	d3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(1024 * 64),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&d3dConstantBuffer)); //GUID of d3d resource

	// Describe and create a constant buffer view.
	CBVDesc = {};
	CBVDesc.BufferLocation = d3dConstantBuffer->GetGPUVirtualAddress();
	CBVDesc.SizeInBytes = (/*sizeof(SceneConstantBuffer)*/ConstantBufferSize + 255) & ~255;	// CB size is required to be 256-byte aligned.
	d3dDevice->CreateConstantBufferView(&CBVDesc, CBVHeap->GetCPUDescriptorHandleForHeapStart());

	// Map and initialize the constant buffer. We don't unmap this until the
	// app closes. Keeping things mapped for the lifetime of the resource is okay.
	CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
	d3dConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&CbvDataBegin));
	memset(InConstantBuffer, 0, /*sizeof(ConstantBufferData)*/ConstantBufferSize);
	memcpy(CbvDataBegin, InConstantBuffer, /*sizeof(ConstantBufferData)*/ConstantBufferSize);
}