// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12Device.cpp - D3D12 logical device. Represents 1 GPU.
//==========================================================
#include "D3D12Device.h"
#include "D3D12Adapter.h"

D3D12Device::D3D12Device(D3D12Adapter& InAdapter)
	: DependencyNode(InAdapter, "D3D12Device")
	, ParentAdapter(InAdapter)
{}

void D3D12Device::Initialize()
{
	if (0)//turgle - take a command line arg for setting up a WARP adapter
	{
		IDXGIAdapter* warpAdapter;
		ParentAdapter.DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

		D3D12CreateDevice(
			warpAdapter,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&D3DDevice)
		);
	}
	else
	{
		D3D12CreateDevice(
			ParentAdapter.DXGIAdapter,
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&D3DDevice)
		);
	}

	for (uint32 i = 0; i < FrameBufferCount; i++)
	{
		// Describe and create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		D3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue));

		D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocators[i]));

		// Create the command list.
		//turgle - have to decide how to manage these for multigpu
		uint32 NodeMask = 0; // MULTIGPUTODO: actually get the node mask set up right for multiple nodes
		D3DDevice->CreateCommandList(NodeMask, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocators[i], nullptr, IID_PPV_ARGS(&CommandList));

		// Command lists are created in the recording state, but there is nothing to record yet. The main loop expects it to be closed, so close it now.
		CommandList->Close();
	}
}


void D3D12Device::Draw()
{
	//-------
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	CommandAllocators[Renderer::FrameIndex]->Reset();

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	CommandList->Reset(CommandAllocators[Renderer::FrameIndex], ParentAdapter.PipelineState);

	// Indicate that the back buffer will be used as a render target.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ParentAdapter.GetCurrentFrameBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(ParentAdapter.RTVHeap->GetCPUDescriptorHandleForHeapStart(), Renderer::FrameIndex, ParentAdapter.RTVDescriptorSize);
	CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	// Record commands.
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	CommandList->ClearRenderTargetView(rtvHandle, clearColor, /*NumRects=*/ 0, /*pRects=*/nullptr);

	// Indicate that the back buffer will now be used to present.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ParentAdapter.GetCurrentFrameBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	CommandList->Close();
	//-------
	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { CommandList };
	CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}