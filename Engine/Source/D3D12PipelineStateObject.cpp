// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12PipelineStateObject.cpp - Represents all the pipeline state, from Input Assembler to OutputMerger, for a draw call.
//==========================================================
/* 
 * Includes the actual bytecode of the shaders bound for a draw call, state for the rasterizer, blending, depth stencil, 
 * (in multigpu) which nodes this state applies to, root signature for bound shaders.
 *
 * GRAPHICS pipeline state objects have a node mask, cached PSO, flags, and then this stuff for the following stages:
 *
 * Input Assembler: 
 *	-PrimitiveTopologyType (primitive adjacency and ordering is set on a command list though)
 *	-Index buffer strip cut value (only used with triangle strips)
 *
 * Shader stages (VS-GS, PS): 
 *	-shader bytecode
 *	-root signature
 *
 * Stream Output: 
 *	-SO desc
 *
 * Rasterizer: 
 *	-Rasterizer state (viewport  and scissor rect set on command list)
 *
 * Output Merger:
 *	-Blend mode
 *	-Number of render targets and their formats
 *	-Multisampling parameters (sample desc)
 *	-Sample mask
 *	-Depth stencil state
 *
 * COMPUTE pipeline state objects just have a root signature, CS bytecode, node mask, cached PSO, and flags.
 */
#include "D3D12PipelineStateObject.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include <D3Dcompiler.h> //turgle - move later
#include "Log.h"
#include <filesystem>

using namespace std::filesystem;

D3D12PipelineStateObject::D3D12PipelineStateObject(D3D12Adapter& InAdapter, D3D12RootSignature& InRootSignature/*, Shaders InShaders*/)
	:  ParentAdapter(InAdapter)
	, RootSignature(InRootSignature)
	//shader bytecode
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
	//turgle - this is lowkey hard-coded. To fix, iterate through and find the Engine and Shaders folders
	path ShaderSourceDirectory = current_path() / path("Engine") / path("Shaders");
	MEGALOGLN(ShaderSourceDirectory);
	std::wstring VSShaderPathString;
	std::wstring PSShaderPathString;
	//turgle - this hot mess is basically example code for myself. I'll move it later.
	for (const directory_entry& directoryEntryIt : recursive_directory_iterator(ShaderSourceDirectory))
	{
		path currentPath = directoryEntryIt.path();
		path pathExtension = currentPath.extension();
		if (pathExtension == L".hlsl")
		{
			MEGALOGLN(currentPath.filename());
			if (currentPath.filename() == L"PixelShader.hlsl")
			{
				PSShaderPathString = currentPath.wstring();
				MEGALOGLN(PSShaderPathString);
			}

			if (currentPath.filename() == L"VertexShader.hlsl")
			{
				VSShaderPathString = currentPath.wstring();
				MEGALOGLN(VSShaderPathString);
			}
		}
	}

	//turgle - all this shader stuff belongs somewhere else
	HRESULT hr = S_OK;
	ID3DBlob* VSErrorBlob = nullptr;
	hr = D3DCompileFromFile(VSShaderPathString.c_str(), /*pDefines=*/nullptr, /*pIncludes=*/nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &VSErrorBlob);
	if (FAILED(hr))
	{
		MEGALOGLN("Failed to compile vertex shader!");
		if (VSErrorBlob)
		{
			MEGALOGLN((char*)VSErrorBlob->GetBufferPointer());
			VSErrorBlob->Release();
		}
	}

	ID3DBlob* PSErrorBlob = nullptr;
	hr = D3DCompileFromFile(PSShaderPathString.c_str(), /*pDefines=*/nullptr, /*pIncludes=*/nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &PSErrorBlob);
	if (FAILED(hr))
	{
		MEGALOGLN("Failed to compile pixel shader!");
		if (PSErrorBlob)
		{
			MEGALOGLN((char*)PSErrorBlob->GetBufferPointer());
			PSErrorBlob->Release();
		}
	}


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
	hr = ParentAdapter.ChildDevice->d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&d3dPipelineState));
}