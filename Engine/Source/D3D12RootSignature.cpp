// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12RootSignature.cpp - 
//==========================================================
#include "D3D12RootSignature.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include <D3Dcompiler.h> //turgle - move later

D3D12RootSignature::D3D12RootSignature(D3D12Adapter& InAdapter)
	: DependencyNode(InAdapter, "D3D12RootSignature")
	, ParentAdapter(InAdapter)
{}

void D3D12RootSignature::Initialize()
{
	// Create an empty root signature.
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(/*numParameters=*/0, /*pParameters=*/nullptr, /*NumStaticSamplers=*/0, /*pStaticSamplers*/nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* signature;
	ID3DBlob* error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	uint32 NodeMask = 0; // MULTIGPUTODO: actually get the node mask set up right for multiple nodes
	ParentAdapter.ChildDevice->d3dDevice->CreateRootSignature(NodeMask, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&d3dRootSignature));
}