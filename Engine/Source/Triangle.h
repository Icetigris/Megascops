// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==============================================================================================================
// Triangle.h - EXTREMELY TEMPORARY class for a triangle entity/object/whatever
//==============================================================================================================
#pragma once
#include "D3D12ConstantBuffer.h"
#include "D3D12VertexBuffer.h"
#include <DirectXMath.h>
#include <SDL.h>

// This will be an entity once I get the database set up.
// This entity will point to tables that represent:
//
// Game/simulation components:
//	-World transform matrix (position, rotation, etc.)
//
// Render components:
//	-Vertex buffer
//	-Index buffer
//	-Constant buffer (depends on world transform matrix from game component)


class Triangle
{
public:
	Triangle() {}

	void Initialize(ID3D12Device* d3dDevice, uint32 NodeMask);
	void Update(SDL_Event Input);

	// Game/simulation components
	DirectX::XMFLOAT4 TrianglePositionOffset;

	// Render components
	D3D12VertexBuffer* VertexBuffer;
	D3D12ConstantBuffer* ConstantBuffer;
};