#pragma once
#include "Vertex.h"
class Component
{
public:
	virtual ~Component() = default;
};

class Mesh : public Component
{
public:
	void LoadMesh();
	vector<Vertex>& GetData() { return m_data; }
	UINT GetByteSize() { return m_byteSize; }
private:
	vector<Vertex> m_data;
	UINT m_byteSize;
};

class Position : public Component
{
public:
	Position() : m_position(0, 0, 0, 0) {};
	Position(float x, float y, float z, float w) : m_position(x, y, z, w) {};
	XMFLOAT4 GetPosition() { return m_position; };
	void SetPosition(XMFLOAT4 position) { m_position = position; };
private:
	XMFLOAT4 m_position;
};

class Velocity : public Component
{
public:
	Velocity() : m_velocity(0, 0, 0, 0) {};
	Velocity(float x, float y, float z, float w) : m_velocity(x, y, z, w) {};
	XMFLOAT4 GetVelocity() { return m_velocity; };
	void SetVelocity(XMFLOAT4 velocity) { m_velocity = velocity; };
private:
	XMFLOAT4 m_velocity;
};


