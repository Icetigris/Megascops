// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12Device.cpp - D3D12 logical device. Represents 1 GPU.
//==========================================================
#include "Renderer.h"
#include "D3D12Device.h"
#include "D3D12Adapter.h"
#include "D3D12RootSignature.h"
#include "D3D12PipelineStateObject.h"
#include "D3D12ConstantBuffer.h" //turgle - pass render components in a less hacky way later
#include "D3D12VertexBuffer.h" //turgle - pass render components in a less hacky way later

D3D12Device::D3D12Device(D3D12Adapter& InAdapter)
	:  ParentAdapter(InAdapter),
	d3dDevice(nullptr),
	CommandAllocators{nullptr},
	CommandQueue(nullptr),
	CommandList(nullptr)
{}

void D3D12Device::Initialize()
{
	if (GCreateWARPAdapter)
	{
		IDXGIAdapter* warpAdapter;
		ParentAdapter.DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

		D3D12CreateDevice(
			warpAdapter,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&d3dDevice)
		);
	}
	else
	{
		D3D12CreateDevice(
			ParentAdapter.DXGIAdapter,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&d3dDevice)
		);
	}

	d3dDevice->SetName(L"D3D12Device");

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.NodeMask = 0; // MULTIGPUTODO: command queues can only be used with 1 NODE

	d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue));
	CommandQueue->SetName(L"DirectCommandQueue");
	
	for (uint32 i = 0; i < FrameBufferCount; i++)
	{
		d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocators[i]));
		CommandAllocators[i]->SetName(L"CommandAllocator");

		// Create the command list.
		//turgle - have to decide how to manage these for multigpu
		uint32 NodeMask = 0; // MULTIGPUTODO: command lists can only be used with 1 NODE
		d3dDevice->CreateCommandList(NodeMask, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocators[i], /*pInitialPipelineState=*/nullptr, IID_PPV_ARGS(&CommandList));
		CommandList->SetName(L"CommandList");

		// Command lists are created in the recording state, but there is nothing to record yet. The main loop expects it to be closed, so close it now.
		CommandList->Close();
	}
}

void D3D12Device::Draw(D3D12ConstantBuffer* ConstantBuffer, D3D12VertexBuffer* VertexBuffer)//turgle - pass render components in a less hacky way later
{
	// Set necessary state.
	CommandList->SetGraphicsRootSignature(ParentAdapter.GetRootSignature());

	ID3D12DescriptorHeap* ppHeaps[] = { ConstantBuffer->CBVHeap };
	CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	CommandList->SetGraphicsRootDescriptorTable(/*RootParameterIndex=*/0, ConstantBuffer->CBVHeap->GetGPUDescriptorHandleForHeapStart());

	// Record commands.

	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->IASetVertexBuffers(/*StartSlot=*/0, /*NumViews=*/1, &VertexBuffer->VertexBufferView);
	CommandList->DrawInstanced(/*VertexCountPerInstance=*/3, /*InstanceCount=*/1, /*StartVertexLocation*/0, /*StartInstanceLocation=*/0);
}