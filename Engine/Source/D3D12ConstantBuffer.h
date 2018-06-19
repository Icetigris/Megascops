// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12ConstantBuffer.h - Constant buffers and constant buffer views.
//==============================================================================================================
#pragma once
#include "Platform.h"
#include <d3d12.h>
#include "d3dx12.h"

class D3D12ConstantBuffer
{
public:
	D3D12ConstantBuffer() {}

	void Initialize(ID3D12Device* d3dDevice, uint32 NodeMask, uint32 ConstantBufferSize, void* InConstantBuffer);
	
	ID3D12Resource* d3dConstantBuffer;
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;

	//turgle - move to it's own thing
	ID3D12DescriptorHeap* CBVHeap;

	//turgle - unhack this
	uint8* CbvDataBegin;
};