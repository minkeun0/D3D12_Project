#pragma once
#include "Component.h"

class Object
{
public:
	Object() = default;
	Object(std::wstring name);

	virtual void OnInit(ID3D12Device* device);
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnDestroy();

	wstring GetObjectName() const;
	float GetSpeed() const;
	
	template<typename T>
	void AddComponent(shared_ptr<T>& component) { m_components[typeid(T).name()] = component; }

	template<typename T>
	shared_ptr<T> GetComponent(){ return static_pointer_cast<T>(m_components[typeid(T).name()]); }

private:
	wstring m_name;
	float m_speed;

	unordered_map<string, shared_ptr<Component>> m_components;

	void SetSpeed(float speed);
};

