#pragma once
#include "Vertex.h"

class Object
{
public:
	Object() = default;
	Object(std::wstring name);

	virtual void OnInit(ID3D12Device* device);
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

	std::wstring GetObjectName() const;
	std::vector<Vertex>* GetMesh();
	UINT GetMeshByteSize() const;
private:
	std::wstring m_name;

	std::vector<Vertex> m_mesh;
	UINT m_meshByteSize;

	void BuildMesh();
};

