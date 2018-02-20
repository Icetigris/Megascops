// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// DependencyNode.h - An object with dependencies.
//==============================================================================================================
#pragma once
#include "Platform.h"
#include <vector>

const uint32 DebugNameMaxLength = 64;

class DependencyNode
{
public:

//probably don't actually want this default constructor
	DependencyNode()
		: bIsReady(false)
		, NodeID(NodeIDCount++)
#ifdef DEBUG
		, DebugName("<unnamed>")
#endif
	{}

	DependencyNode(DependencyNode& InDependency, const char* InDebugName);
	DependencyNode(std::vector<DependencyNode> InDependencies, const char* InDebugName);
	virtual ~DependencyNode(){}

#ifdef DEBUG
	const char* DebugName;
#endif
	uint64 GetID() const { return NodeID; }
	// Return a read-only list of this object's dependencies' IDs.
	const std::vector<uint64>& GetDependencies() const { return Dependencies; }

	// Dependencies should call this to check if this object is safe to use.
	bool IsReady() const { return bIsReady; }

	// Call this when an object is finished doing stuff so whatever depends on it can use it.
	void MarkAsReady() { bIsReady = true; }

	void AddDependency(DependencyNode InDependency);

private:
	// Unique identifier, might make this an actual guid later like when I need thread safety or something
	uint64 NodeID;

	// IDs of the objects this depends on.
	std::vector<uint64> Dependencies;

	// If true, any object that depends on this one can safely consider this dependency as ready for use.
	bool bIsReady;

	// Just increment for a new ID
	static uint64 NodeIDCount;
};