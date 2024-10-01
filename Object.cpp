#include "Object.h"

Object::Object(std::wstring name) :
    m_name(name)
{
}

void Object::OnInit(ID3D12Device* device)
{

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