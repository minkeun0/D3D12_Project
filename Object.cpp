#include "stdafx.h"
#include "Object.h"

Object::Object(std::wstring name) :
    m_name(name)
{
}

void Object::OnInit(ID3D12Device* device)
{
    BuildMesh();
}

void Object::OnUpdate()
{
}

void Object::OnRender()
{
}

void Object::OnDestroy()
{
}

std::wstring Object::GetObjectName() const
{
    return m_name;
}

std::vector<Vertex>* Object::GetMesh()
{
    return &m_mesh;
}

UINT Object::GetMeshByteSize() const
{
    return m_meshByteSize;
}

void Object::BuildMesh()
{
    std::vector<Vertex> tmp = {
        { { -0.5f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
        { { 0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
    };
    m_mesh.assign(tmp.begin(), tmp.end());
    m_meshByteSize = m_mesh.size() * sizeof(Vertex);
}
