#pragma once
#include "stdafx.h"
#include "Component.h"
#include <variant>

class GameTimer;
class Scene;

class Object
{
public:
	Object() = default;
	Object(Scene* root);
	virtual ~Object() = default;

	//virtual void OnInit(ID3D12Device* device);
	virtual void OnUpdate(GameTimer& gTimer) = 0;
	virtual void OnRender(ID3D12Device* device, ID3D12GraphicsCommandList * commandList) = 0;
	//virtual void OnDestroy();

	template<typename T>
	void AddComponent(T&& component) { m_components.emplace(typeid(T).name(), move(component)); }

	template <typename T>
	T& GetComponent() { return get<T>(m_components.at(typeid(T).name())); }

protected:
	Scene* m_root;
	unordered_map<string, ComponentVariant> m_components;
};

class PlayerObject : public Object
{
public:
	PlayerObject() = default;
	PlayerObject(Scene* root);
	virtual void OnUpdate(GameTimer& gTimer);
	virtual void OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void OnKeyboardInput(const GameTimer& gTimer);
};

class CameraObject : public Object
{
public:
	CameraObject() = default;
	CameraObject(float radius, Scene* root);
	virtual void OnUpdate(GameTimer& gTimer);
	virtual void OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	void OnMouseInput(WPARAM wParam, int x, int y);
	void SetXMMATRIX(XMMATRIX m);
	XMMATRIX& GetXMMATRIX();
private:
	int mLastPosX;
	int mLastPosY;
	float mTheta;
	float mPhi;
	float mRadius;
	XMFLOAT4X4 mViewMatrix;
};

class TestObject : public Object
{
public:
	TestObject() = default;
	TestObject(Scene* root);
	virtual void OnUpdate(GameTimer& gTimer);
	virtual void OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
};

using ObjectVariant = variant<PlayerObject, CameraObject, TestObject>;