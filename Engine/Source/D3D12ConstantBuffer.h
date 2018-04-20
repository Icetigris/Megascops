// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12ConstantBuffer.h - Constant buffers and constant buffer views.
//==============================================================================================================
#pragma once
#include "DependencyNode.h"
#include <d3d12.h>
#include "d3dx12.h"
#include "Renderer.h"
#include <DirectXMath.h> //turgle - move later

//turgle - unhack this
struct SceneConstantBuffer
{
	DirectX::XMFLOAT4 offset;
};

// Dependencies: D3D12Device
class D3D12Device;
class D3D12ConstantBuffer : public DependencyNode
{
public:
	D3D12ConstantBuffer(D3D12Device& InDevice);

	void Initialize();
	
	ID3D12Device* d3dDevice; //probably don't need this? should be able to just pass it into Initialize() and check if the device is ready?
	ID3D12Resource* ConstantBuffer;
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;

	//turgle - move to it's own thing
	ID3D12DescriptorHeap* CBVHeap;

	//turgle - unhack this
	SceneConstantBuffer ConstantBufferData;
	uint8* CbvDataBegin;
};