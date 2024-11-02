#include "MeshManager.h"

MeshManager::MeshManager()
{
	m_vertexBuffer = make_unique<vector<Vertex>>();
	m_indexBuffer = make_unique<vector<uint16_t>>();
	BuildBox(1.0f, 1.0f, 1.0f);
}

Vertex* MeshManager::GetVertexData()
{
	return m_vertexBuffer->data();
}

uint16_t* MeshManager::GetIndexData()
{
	return m_indexBuffer->data();
}

UINT MeshManager::GetVertexBufferByteSize()
{
	return m_vertexBuffer->size() * sizeof(Vertex);
}

UINT MeshManager::GetindexBufferByteSize()
{
	return m_indexBuffer->size() * sizeof(uint16_t);
}

SubMeshData& MeshManager::GetSubMeshData(wstring name)
{
	return m_subMeshData[name];
}

void MeshManager::BuildBox(float width, float height, float depth)
{
	Vertex v[24];

	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Fill in the front face vertex data.
	v[0] = { {-w2, -h2, -d2} , {0.0f, 3.0f} };
	v[1] = { {-w2, +h2, -d2} , {0.0f, 0.0f} };
	v[2] = { {+w2, +h2, -d2} , {3.0f, 0.0f} };
	v[3] = { {+w2, -h2, -d2} , {3.0f, 3.0f} };

	// Fill in the back face vertex data.
	v[4] = { {-w2, -h2, +d2}, {1.0f, 1.0f} };
	v[5] = { {+w2, -h2, +d2}, {0.0f, 1.0f} };
	v[6] = { {+w2, +h2, +d2}, {0.0f, 0.0f} };
	v[7] = { {-w2, +h2, +d2}, {1.0f, 0.0f} };

	// Fill in the top face vertex data.
	v[8] = { {-w2, +h2, -d2}, {0.0f, 1.0f} };
	v[9] = { {-w2, +h2, +d2}, {0.0f, 0.0f} };
	v[10] = { {+w2, +h2, +d2}, {1.0f, 0.0f} };
	v[11] = { {+w2, +h2, -d2}, {1.0f, 1.0f} };

	// Fill in the bottom face vertex data.
	v[12] = { {-w2, -h2, -d2}, {1.0f, 1.0f} };
	v[13] = { {+w2, -h2, -d2}, {0.0f, 1.0f} };
	v[14] = { {+w2, -h2, +d2}, {0.0f, 0.0f} };
	v[15] = { {-w2, -h2, +d2}, {1.0f, 0.0f} };

	// Fill in the left face vertex data.
	v[16] = { {-w2, -h2, +d2}, {0.0f, 1.0f} };
	v[17] = { {-w2, +h2, +d2}, {0.0f, 0.0f} };
	v[18] = { {-w2, +h2, -d2}, {1.0f, 0.0f} };
	v[19] = { {-w2, -h2, -d2}, {1.0f, 1.0f} };

	// Fill in the right face vertex data.
	v[20] = { {+w2, -h2, -d2}, {0.0f, 1.0f} };
	v[21] = { {+w2, +h2, -d2}, {0.0f, 0.0f} };
	v[22] = { {+w2, +h2, +d2}, {1.0f, 0.0f} };
	v[23] = { {+w2, -h2, +d2}, {1.0f, 1.0f} };

	UINT vertexCountPerInstance = _countof(v);
	UINT baseVertexLocation = m_vertexBuffer->size();
	m_vertexBuffer->insert(m_vertexBuffer->end(), &v[0], &v[vertexCountPerInstance]);

	uint16_t i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	UINT indexCountPerInstance = _countof(i);
	UINT statIndexLocation = m_indexBuffer->size();
	m_indexBuffer->insert(m_indexBuffer->end(),&i[0], &i[indexCountPerInstance]);

	m_subMeshData[L"Box"] = SubMeshData{ vertexCountPerInstance, indexCountPerInstance, statIndexLocation, baseVertexLocation };
}
