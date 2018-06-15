// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12Device.cpp - D3D12 logical device. Represents 1 GPU.
//==========================================================
#include "Renderer.h"
#include "D3D12Device.h"
#include "D3D12Adapter.h"
#include "D3D12RootSignature.h"
#include "D3D12PipelineStateObject.h"
#include "D3D12VertexBuffer.h"
#include "D3D12ConstantBuffer.h"

D3D12Device::D3D12Device(D3D12Adapter& InAdapter)
	:  ParentAdapter(InAdapter)
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

	//turgle move later
	VertexBuffer = new D3D12VertexBuffer(*this);
	VertexBuffer->Initialize();
	//turgle move later
	ConstantBuffer = new D3D12ConstantBuffer(*this);
	ConstantBuffer->Initialize();

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
//turgle move later
CD3DX12_VIEWPORT Viewport(0.0f, 0.0f, static_cast<float>(WinWidth), static_cast<float>(WinHeight));
CD3DX12_RECT ScissorRect(0, 0, static_cast<LONG>(WinWidth), static_cast<LONG>(WinHeight));
void D3D12Device::Draw()
{
	const float translationSpeed = 0.005f;
	const float offsetBounds = 1.25f;

	ConstantBuffer->ConstantBufferData.offset.x += translationSpeed;
	if (ConstantBuffer->ConstantBufferData.offset.x > offsetBounds)
	{
		ConstantBuffer->ConstantBufferData.offset.x = -offsetBounds;
	}
	memcpy(ConstantBuffer->CbvDataBegin, &ConstantBuffer->ConstantBufferData, sizeof(ConstantBuffer->ConstantBufferData));
	//-------
	// Command list allocators can only be reset when the associated 
	// command lists have finished execution on the GPU; apps should use 
	// fences to determine GPU execution progress.
	CommandAllocators[Renderer::FrameIndex]->Reset();

	// However, when ExecuteCommandList() is called on a particular command 
	// list, that command list can then be reset at any time and must be before 
	// re-recording.
	CommandList->Reset(CommandAllocators[Renderer::FrameIndex], ParentAdapter.PipelineStateObject->d3dPipelineState);
	
	// Set necessary state.
	CommandList->SetGraphicsRootSignature(ParentAdapter.RootSignature->d3dRootSignature);

	ID3D12DescriptorHeap* ppHeaps[] = { ConstantBuffer->CBVHeap };
	CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	CommandList->SetGraphicsRootDescriptorTable(0, ConstantBuffer->CBVHeap->GetGPUDescriptorHandleForHeapStart());
	CommandList->RSSetViewports(1, &Viewport);
	CommandList->RSSetScissorRects(1, &ScissorRect);

	// Indicate that the back buffer will be used as a render target.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ParentAdapter.GetCurrentFrameBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(ParentAdapter.RTVHeap->GetCPUDescriptorHandleForHeapStart(), Renderer::FrameIndex, ParentAdapter.RTVDescriptorSize);
	CommandList->OMSetRenderTargets(/*NumRenderTargetDescriptors=*/1, &rtvHandle, /*RTsSingleHandleToDescriptorRange=*/FALSE, /*pDepthStencilDescriptor*/nullptr);

	// Record commands.
	const float clearColor[] = { 0.2f, 0.0f, 0.4f, 1.0f };
	CommandList->ClearRenderTargetView(rtvHandle, clearColor, /*NumRects=*/ 0, /*pRects=*/nullptr);

	CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	CommandList->IASetVertexBuffers(/*StartSlot=*/0, /*NumViews=*/1, &VertexBuffer->VertexBufferView);
	CommandList->DrawInstanced(/*VertexCountPerInstance=*/3, /*InstanceCount=*/1, /*StartVertexLocation*/0, /*StartInstanceLocation=*/0);

	// Indicate that the back buffer will now be used to present.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ParentAdapter.GetCurrentFrameBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	CommandList->Close();
	//-------
	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { CommandList };
	CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}