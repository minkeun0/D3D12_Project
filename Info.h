#pragma once
#include "stdafx.h"

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	XMFLOAT4 weight;
	int boneIndex[4];
};

struct ObjectCB
{
    XMFLOAT4X4 world;
	XMFLOAT4X4 finalTransform[90];
	int isAnimate;
	int padding0[3];
	float powValue;
	float ambiantValue;
	float padding1[2];
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
	UINT startIndexLocation;
	UINT baseVertexLocation;
};