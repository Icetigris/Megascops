// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// GPUNodeMask.h - Wrapper for the bitmask used to tell D3D12 which GPU you're talking about.
//==============================================================================================================
#pragma once
#include "Platform.h"
#include "PlatformIntrinsics.h"
#include <assert.h>

static const int32 MAX_GPU_COUNT = 8;

class GPUNodeMask
{
public:
	GPUNodeMask(const GPUNodeMask&) = default;
	GPUNodeMask() { ClearMask(); }

	void ClearMask()
	{
		*this = NoGPUs();
	}

	static GPUNodeMask NoGPUs()
	{
		return GPUNodeMask(0);
	}

	static GPUNodeMask AllGPUs()
	{
		return GPUNodeMask((1 << MAX_GPU_COUNT) - 1);
	}

	bool operator ==(const GPUNodeMask& RHS) const
	{
		return NodeMask == RHS.NodeMask;
	}

	bool operator !=(const GPUNodeMask& RHS) const
	{
		return NodeMask != RHS.NodeMask;
	}

	static GPUNodeMask CreateMaskFromIndex(int32 NodeIndex)
	{
		assert(NodeIndex >= 0 && NodeIndex < MAX_GPU_COUNT);
		return GPUNodeMask(1 << NodeIndex);
	}

	void AddGPUByIndex(int32 NodeIndex)
	{
		assert(NodeIndex >= 0 && NodeIndex < MAX_GPU_COUNT);
		NodeMask |= 1 << NodeIndex;
	}

	void RemoveGPUByIndex(int32 NodeIndex)
	{
		assert(NodeIndex >= 0 && NodeIndex < MAX_GPU_COUNT);
		NodeMask &= ~(1 << NodeIndex);
	}

	void AddGPUsFromMask(GPUNodeMask InMask)
	{
		NodeMask |= InMask.NodeMask;
	}

	void RemoveGPUsFromMask(GPUNodeMask InMask)
	{
		NodeMask &= ~InMask.NodeMask;
	}

	int32 GPUCount() const
	{
		return PopCnt(NodeMask);
	}

	// For getting the raw mask the d3d12 functions need
	uint32 GetMask() const
	{
		return NodeMask;
	}

private:
	explicit GPUNodeMask(uint32 InNodeMask)
		: NodeMask(InNodeMask)
	{
		assert(NodeMask < (1 << MAX_GPU_COUNT));
	}
	uint32 NodeMask;
};