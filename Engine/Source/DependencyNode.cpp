// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// DependencyNode.cpp - Object with dependencies.
//==========================================================
#include "DependencyNode.h"

// device depends on adapter

uint64 DependencyNode::NodeIDCount = 0;

DependencyNode::DependencyNode(DependencyNode& InDependency, const char* InDebugName)
	: bIsReady(false)
	, NodeID(NodeIDCount++)
#ifdef DEBUG
	, DebugName(InDebugName)
#endif
{
	Dependencies.push_back(InDependency.GetID());
}

DependencyNode::DependencyNode(std::vector<DependencyNode> InDependencies, const char* InDebugName)
	: bIsReady(false)
	, NodeID(NodeIDCount++)
#ifdef DEBUG
	, DebugName(InDebugName)
#endif
{
	for each (DependencyNode dependency in InDependencies)
	{
		Dependencies.push_back(dependency.GetID());
	}
}

void DependencyNode::AddDependency(DependencyNode InDependency)
{
	Dependencies.push_back(InDependency.GetID());
}