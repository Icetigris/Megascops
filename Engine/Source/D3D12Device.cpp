// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12Device.cpp - D3D12 logical device. Represents 1 GPU.
//==========================================================
#include "D3D12Device.h"
#include "D3D12Adapter.h"
#include <D3Dcompiler.h> //turgle - move later
#include <DirectXMath.h> //turgle - move later

D3D12Device::D3D12Device(D3D12Adapter& InAdapter)
	: DependencyNode(InAdapter, "D3D12Device")
	, ParentAdapter(InAdapter)
{}

// !!!!! BIG MESS START !!!!!!
//a bunch of temporary functions full of code I jacked from the d3d12 samples
//everything in here is destined for classes that live in other files
//void CreateRootSignature(ID3D12Device* D3DDevice)
//void CreatePipelineStateObject(ID3D12Device* D3DDevice, ID3D12PipelineState* PipelineState)

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

ID3D12Resource* VertexBuffer;
D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
float AspectRatio;
void CreateVertexBuffer(ID3D12Device* D3DDevice)
{
	// Create the vertex buffer.
	{
		// Define the geometry for a triangle.
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * AspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
			{ { 0.25f, -0.25f * AspectRatio, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
			{ { -0.25f, -0.25f * AspectRatio, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
		};

		const UINT vertexBufferSize = sizeof(triangleVertices);

		// Note: using upload heaps to transfer static data like vert buffers is not 
		// recommended. Every time the GPU needs it, the upload heap will be marshalled 
		// over. Please read up on Default Heap usage. An upload heap is used here for 
		// code simplicity and because there are very few verts to actually transfer.
		D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&VertexBuffer));

		// Copy the triangle data to the vertex buffer.
		UINT8* pVertexDataBegin;
		CD3DX12_RANGE readRange(0, 0);		// We do not intend to read from this resource on the CPU.
		VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin));
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		VertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
		VertexBufferView.StrideInBytes = sizeof(Vertex);
		VertexBufferView.SizeInBytes = vertexBufferSize;
	}
}
// !!!!! BIG MESS FINISH !!!!!!

void D3D12Device::Initialize()
{
	if (0)//turgle - take a command line arg for setting up a WARP adapter
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

	d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue));
	CommandQueue->SetName(L"DirectCommandQueue");

	for (uint32 i = 0; i < FrameBufferCount; i++)
	{
		d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&CommandAllocators[i]));
		CommandAllocators[i]->SetName(L"CommandAllocator");

		// Create the command list.
		//turgle - have to decide how to manage these for multigpu
		uint32 NodeMask = 0; // MULTIGPUTODO: actually get the node mask set up right for multiple nodes
		d3dDevice->CreateCommandList(NodeMask, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAllocators[i], /*pInitialPipelineState=*/nullptr, IID_PPV_ARGS(&CommandList));
		CommandList->SetName(L"CommandList");

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
	
	// Set necessary state.
	//CommandList->SetGraphicsRootSignature(RootSignature);
	//CommandList->RSSetViewports(1, &Viewport);
	//CommandList->RSSetScissorRects(1, &ScissorRect);

	// Indicate that the back buffer will be used as a render target.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ParentAdapter.GetCurrentFrameBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(ParentAdapter.RTVHeap->GetCPUDescriptorHandleForHeapStart(), Renderer::FrameIndex, ParentAdapter.RTVDescriptorSize);
	CommandList->OMSetRenderTargets(/*NumRenderTargetDescriptors=*/1, &rtvHandle, /*RTsSingleHandleToDescriptorRange=*/FALSE, /*pDepthStencilDescriptor*/nullptr);

	// Record commands.
	const float clearColor[] = { 0.2f, 0.0f, 0.4f, 1.0f };
	CommandList->ClearRenderTargetView(rtvHandle, clearColor, /*NumRects=*/ 0, /*pRects=*/nullptr);

	//CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//CommandList->IASetVertexBuffers(/*StartSlot=*/0, /*NumViews=*/1, &vertexBufferView);
	//CommandList->DrawInstanced(/*VertexCountPerInstance=*/3, /*InstanceCount=*/1, /*StartVertexLocation*/0, /*StartInstanceLocation=*/0);

	// Indicate that the back buffer will now be used to present.
	CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(ParentAdapter.GetCurrentFrameBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	CommandList->Close();
	//-------
	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { CommandList };
	CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}