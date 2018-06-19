// Copyright 2018 Elizabeth Baumel. All rights reserved.
//==========================================================
// Triangle.cpp - EXTREMELY TEMPORARY class for a triangle entity/object/whatever
//==========================================================
#include "Triangle.h"
#include "Renderer.h" //needed for AspectRatio

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

void Triangle::Initialize(ID3D12Device* d3dDevice, uint32 NodeMask)
{
	// Define the geometry for a triangle.
	Vertex triangleVertices[] =
	{
		{ { 0.0f, 0.25f * AspectRatio, 0.0f },{ 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f * AspectRatio, 0.0f },{ 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f * AspectRatio, 0.0f },{ 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	const uint32 vertexBufferSize = sizeof(triangleVertices);
	const uint32 strideInBytes = sizeof(Vertex);

	VertexBuffer = new D3D12VertexBuffer();
	VertexBuffer->Initialize(d3dDevice, vertexBufferSize, strideInBytes, triangleVertices);
	
	TrianglePositionOffset = DirectX::XMFLOAT4(0.0, 0.0, 0.0, 0.0);
	ConstantBuffer = new D3D12ConstantBuffer();
	ConstantBuffer->Initialize(d3dDevice, NodeMask, sizeof(TrianglePositionOffset), &TrianglePositionOffset);
}

void Triangle::Update(SDL_Event Input)
{
	//turgle - triangle position (pretend we loaded this from the database instead of hardcoding it)
	const float translationSpeed = 0.005f;
	const float offsetBounds = 1.25f;

	if (Input.key.keysym.sym == SDLK_LEFT)
	{
		TrianglePositionOffset.x -= translationSpeed;
	}
	if (Input.key.keysym.sym == SDLK_RIGHT)
	{
		TrianglePositionOffset.x += translationSpeed;
	}
	if (Input.key.keysym.sym == SDLK_UP)
	{
		TrianglePositionOffset.y += translationSpeed;
	}
	if (Input.key.keysym.sym == SDLK_DOWN)
	{
		TrianglePositionOffset.y -= translationSpeed;
	}

	//keep triangle within window
	if (TrianglePositionOffset.x > offsetBounds)
	{
		TrianglePositionOffset.x = -offsetBounds;
	}
	if (TrianglePositionOffset.x < -offsetBounds)
	{
		TrianglePositionOffset.x = offsetBounds;
	}

	if (TrianglePositionOffset.y > offsetBounds)
	{
		TrianglePositionOffset.y = -offsetBounds;
	}
	if (TrianglePositionOffset.y < -offsetBounds)
	{
		TrianglePositionOffset.y = offsetBounds;
	}

	// copy updated position to constant buffer render component
	memcpy(ConstantBuffer->CbvDataBegin, &TrianglePositionOffset, sizeof(TrianglePositionOffset));
}