// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12PipelineStateObject.cpp - 
//==========================================================
#include "D3D12PipelineStateObject.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include <D3Dcompiler.h> //turgle - move later

D3D12PipelineStateObject::D3D12PipelineStateObject(D3D12Adapter& InAdapter, D3D12RootSignature& InRootSignature/*, Shaders InShaders*/)
	:  ParentAdapter(InAdapter)
	, RootSignature(InRootSignature)
{
	std::vector<DependencyNode> SetupDependencies;
	SetupDependencies.push_back(InAdapter);
	SetupDependencies.push_back(InRootSignature);
	DependencyNode(SetupDependencies, "D3D12PipelineStateObject");
}

void D3D12PipelineStateObject::Initialize()
{
	// Create the pipeline state, which includes compiling and loading shaders.
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	uint32 compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	uint32 compileFlags = 0;
#endif
	//turgle - all this shader stuff belongs somewhere else
	D3DCompileFromFile(L"D:/Megascops/Engine/Shaders/VertexShader.hlsl", /*pDefines=*/nullptr, /*pIncludes=*/nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, /*ppErrorMsgs=*/nullptr);
	D3DCompileFromFile(L"D:/Megascops/Engine/Shaders/PixelShader.hlsl", /*pDefines=*/nullptr, /*pIncludes=*/nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, /*ppErrorMsgs=*/nullptr);

	//turgle - input layouts should go with geometry shit
	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = RootSignature.d3dRootSignature;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader); //turgle - all this shader stuff belongs somewhere else
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader); //turgle - all this shader stuff belongs somewhere else
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	//psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.NodeMask = 0; //MULTIGPUTODO - pipeline state objects can be used with more than one node at a time
	//psoDesc.CachedPSO;
	//psoDesc.Flags = 0;
	ParentAdapter.ChildDevice->d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&d3dPipelineState));
}