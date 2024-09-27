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
    SetSpeed(0.005f);
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

float Object::GetSpeed() const
{
    return m_speed;
}

void Object::SetSpeed(float speed)
{
    float maxValue = 1.25f;
    m_speed += speed;
    if (m_speed > maxValue) {
        m_speed -= maxValue;
    }
}