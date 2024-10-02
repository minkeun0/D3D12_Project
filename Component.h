#pragma once
#include "stdafx.h"
#include "Info.h"

class Component
{
public:
	virtual ~Component() = default;
};

class Mesh : public Component
{ 
public:
	Mesh(wstring name, SubMeshData subMeshData) : m_name(name), m_subMeshData(subMeshData) {}
	SubMeshData& GetSubMeshData() { return m_subMeshData; }
	wstring GetName() { return m_name; }
private:
	wstring m_name;
	SubMeshData m_subMeshData;
};

class Position : public Component
{
public:
	Position() : m_position(0, 0, 0, 0) {};
	Position(float x, float y, float z, float w) : m_position(x, y, z, w) {}
	XMVECTOR GetPosition() { return XMLoadFloat4(&m_position); }
	void SetPosition(XMVECTOR position) { XMStoreFloat4(&m_position, position); }
private:
	XMFLOAT4 m_position;
};

class Velocity : public Component
{
public:
	Velocity() : m_velocity(0, 0, 0, 0) {}
	Velocity(float x, float y, float z, float w) : m_velocity(x, y, z, w) {}
	XMVECTOR GetVelocity() { return XMLoadFloat4(&m_velocity); }
	void SetVelocity(XMVECTOR velocity) { XMStoreFloat4(&m_velocity ,velocity); }
private:
	XMFLOAT4 m_velocity;
};

class Rotation : public Component
{
public:
	Rotation() : m_rotation(0, 0, 0, 0) {}
	Rotation(float x, float y, float z, float w) : m_rotation(x, y, z, w) {}
	XMVECTOR GetRotation() { return XMLoadFloat4(&m_rotation); }
	void SetRotation(XMVECTOR rotation) { XMStoreFloat4(&m_rotation, rotation); }
private:
	XMFLOAT4 m_rotation;
};

class Rotate : public Component
{
public:
	Rotate() : m_rotate(0, 0, 0, 0) {}
	Rotate(float x, float y, float z, float w) : m_rotate(x, y, z, w) {}
	XMVECTOR GetRotate() { return XMLoadFloat4(&m_rotate); }
	void SetRotate(XMVECTOR rotate) { XMStoreFloat4(&m_rotate, rotate); }
private:
	XMFLOAT4 m_rotate;
};

class TransfromMatrix : public Component
{
public:
	XMMATRIX GetMatrix() { return XMLoadFloat4x4(&m_Matrix); }
	void SetMatrix(XMMATRIX matrix) { XMStoreFloat4x4(&m_Matrix, matrix); }
private:
	XMFLOAT4X4 m_Matrix;
};
