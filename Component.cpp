#include "Component.h"

void Mesh::LoadMesh()
{
	float size = 0.1f;
	m_data.push_back(Vertex{ { size, -size, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } });
	m_data.push_back(Vertex{ { -size, -size, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } });
	m_data.push_back(Vertex{ { -size, size, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } });
	m_data.push_back(Vertex{ { -size, size, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } });
	m_data.push_back(Vertex{ { size, size, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } });
	m_data.push_back(Vertex{ { size, -size, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } });

	m_byteSize = m_data.size() * sizeof(Vertex);
}
