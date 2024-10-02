#pragma once
#include "stdafx.h"
#include "Info.h"

class MeshManager
{
public:
	MeshManager();
	~MeshManager() {};
	Vertex* GetVertexData();
	uint16_t* GetIndexData();
	UINT GetVertexBufferByteSize();
	UINT GetindexBufferByteSize();
	SubMeshData& GetSubMeshData(wstring name);
private:
	unique_ptr<vector<Vertex>> m_vertexBuffer;
	unique_ptr<vector<uint16_t>> m_indexBuffer;
	unordered_map<wstring, SubMeshData> m_subMeshData;

	void BuildBox(float width, float height, float depth);
	// 메쉬 추가 예정
};

