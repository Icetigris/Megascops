// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// D3D12Adapter.cpp - D3D12 hardware adapter. Can represent 1 or more devices.
//==========================================================
#include "Renderer.h"
#include "D3D12Adapter.h"
#include "D3D12Device.h"
#include "Log.h"
#include "D3D12RootSignature.h"
#include "D3D12PipelineStateObject.h"
#include <vector>

//also copied from the d3d12 samples lol
// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
void D3D12Adapter::Initialize()
{
	IDXGIAdapter4* TempAdapter;
	std::vector<DXGI_ADAPTER_DESC3> FoundAdapterDescs;

	uint32 dxgiFactoryFlags = 0;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&DXGIFactory));

	if(GEnableD3DDebug)
	{
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

			MEGALOGLN("D3DDebug Layer Enabled.");
		}
	}

	uint32 adapterIndex = 0;
	for (adapterIndex = 0; DXGI_ERROR_NOT_FOUND != DXGIFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&TempAdapter)); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC3 desc;
		TempAdapter->GetDesc3(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			// Don't select the Basic Render Driver adapter.
			// If you want a software adapter, pass in "/warp" on the command line.
			continue;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(TempAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			D3D12_FEATURE_DATA_D3D12_OPTIONS D3D12Capabilites;
			memset(&D3D12Capabilites, 0, sizeof(D3D12Capabilites));
			MEGALOGLN("Found hardware adapter: ");
			MEGALOGLN(desc.Description);
			MEGALOGLN("Dedicated video memory: " << desc.DedicatedVideoMemory / 1048576 << " MB");
			MEGALOGLN("Dedicated system memory: " << desc.DedicatedSystemMemory / 1048576 << " MB");
			MEGALOGLN("Shared system memory: " << desc.SharedSystemMemory / 1048576 << " MB");
			FoundAdapterDescs.push_back(desc);
			break; //turgle - delete later when you're enuming multiple adapters properly
		}
	}

	if (FoundAdapterDescs.size() < 1)
	{
		MEGALOGLN("Could not find a hardware adapter that supports D3D12.");
	}
	else if (adapterIndex > 0)
	{
		MEGALOGLN("Found " << adapterIndex + 1 << " hardware adapters.");
	}
	else
	{
		MEGALOGLN("Found 1 hardware adapter.");
	}

	DXGIAdapter = TempAdapter;
	
	//turgle - move all the shit beneath this line somewhere else
	// might need a factory for devices too
	RootDevice = new D3D12Device(*this); //turgle this is messy and I don't like it but I want this to run before I unfuck it

	RootDevice->Initialize(); // MULTIGPUTODO: for EACH DEVICE

	//turgle move later
	RootSignature = new D3D12RootSignature();
	RootSignature->Initialize(RootDevice->d3dDevice, /*NodeMask=*/0);

	PipelineStateObject = new D3D12PipelineStateObject();
	PipelineStateObject->Initialize(RootDevice->d3dDevice, RootSignature->d3dRootSignature, /*NodeMask=*/0);
}

ID3D12PipelineState* D3D12Adapter::GetPipelineState() const
{
	return PipelineStateObject->d3dPipelineState;
}

ID3D12RootSignature* D3D12Adapter::GetRootSignature() const
{
	return RootSignature->d3dRootSignature;
}