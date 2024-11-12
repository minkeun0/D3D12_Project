#pragma once
#include "stdafx.h"

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 uv;
};

struct ObjectCB
{
    XMFLOAT4X4 world;
};

struct CommonCB
{
    XMFLOAT4X4 view;
    XMFLOAT4X4 proj;
};

struct SubMeshData
{
	UINT vertexCountPerInstance;
	UINT indexCountPerInstance;
	UINT startVertexLocation;
	UINT statIndexLocation;
	UINT baseVertexLocation;
};