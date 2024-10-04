#pragma once
#include "stdafx.h"
#include "Component.h"

class GameTimer;

class Object
{
public:
	Object() = default;
	Object(std::wstring name);
	virtual ~Object() {};

	//virtual void OnInit(ID3D12Device* device);
	virtual void OnUpdate(GameTimer& gTimer) = 0;
	virtual void OnRender(ID3D12GraphicsCommandList* commandList) = 0;
	//virtual void OnDestroy();

	wstring GetObjectName();

	template<typename T>
	void AddComponent(shared_ptr<T>& component) { m_components[typeid(T).name()] = component; }

	template<typename T>
	bool GetComponent(shared_ptr<T>* component){ 
		auto it = m_components.find(typeid(T).name());
		if (it == m_components.end()) {
			return false;
		}
		*component = static_pointer_cast<T>(it->second);
		return true;
	}
protected:
	wstring m_name;
	unordered_map<string, shared_ptr<Component>> m_components;
};

class PlayerObject : public Object
{
public:
	PlayerObject(wstring name) : Object(name) {}
	virtual void OnUpdate(GameTimer& gTimer);
	virtual void OnRender(ID3D12GraphicsCommandList* commandList);
	XMMATRIX OnKeyboardInput(const GameTimer& gTimer);
};

class SampleObject : public Object
{
public:
	SampleObject(wstring name) : Object(name) {}
	virtual void OnUpdate(GameTimer& gTimer);
	virtual void OnRender(ID3D12GraphicsCommandList* commandList);
};