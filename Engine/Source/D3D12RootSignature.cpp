// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// D3D12RootSignature.h - Defines which resources the shader needs bound to the pipeline.
//==============================================================================================================
/*
 * This is the thing that defines what resources you need bound and which PSOs you can use with a shader.
 * It's kinda like a function signature, but for a shader. Each entry is a root parameter, you fill root parameters with root arguments.
 * Root arguments can contain inlined root constants, inlined descriptors, and descriptor tables (pointers to a range of descriptors in a descriptor heap).
 * Root constants are basically inlined constant buffers. NO BOUNDS CHECKING
 * Root descriptors are inlined CBVs and 1D raw or structured SRV or UAV buffers. Use this for frequently-accessed buffers of these types. NO BOUNDS CHECKING
 * Descriptor tables are for everything else. Most stuff goes in there, like 2D texture SRVs, samplers, etc. These do specify a size for bounds checking.
 */
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
	// Create a root signature consisting of a descriptor table with a single CBV.
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		if (ParentAdapter.ChildDevice->d3dDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
		CD3DX12_ROOT_PARAMETER1 rootParameters[1];

		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

		// Allow input layout and deny unnecessary access to certain pipeline stages.
		D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

		ID3DBlob* signature;
		ID3DBlob* error;
		D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
		ParentAdapter.ChildDevice->d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&d3dRootSignature));
	}

	//// Create an empty root signature.
	//CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	//rootSignatureDesc.Init(/*numParameters=*/0, /*pParameters=*/nullptr, /*NumStaticSamplers=*/0, /*pStaticSamplers*/nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	//ID3DBlob* signature;
	//ID3DBlob* error;
	//D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	//uint32 NodeMask = 0; // MULTIGPUTODO: actually get the node mask set up right for multiple nodes
	//ParentAdapter.ChildDevice->d3dDevice->CreateRootSignature(NodeMask, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&d3dRootSignature));
}