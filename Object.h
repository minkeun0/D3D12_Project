#pragma once
#include "stdafx.h"
#include "Component.h"

class GameTimer;
class Scene;
class Object
{
public:
	virtual ~Object();
	Object(Scene* root);
	virtual void OnUpdate(GameTimer& gTimer);
	virtual void LateUpdate(GameTimer& gTimer);
	virtual void OnRender(ID3D12Device* device, ID3D12GraphicsCommandList * commandList);
	void BuildConstantBuffer(ID3D12Device* device);
	void AddComponent(Component* component);
	Scene* GetParent() { return m_parent; }
	void ProcessAnimation(GameTimer& gTimer);
	template <typename T>
	T* GetComponent() 
	{
		T* temp = nullptr;
		for (Component* component : m_components){
			temp = dynamic_cast<T*>(component);
			if (temp) break;
		}
		return temp; 
	}

protected:
	Scene* m_parent;
	vector<Component*> m_components;

	// 오브젝트 마다 독립적인 CB
	UINT8* m_mappedData;
	ComPtr<ID3D12Resource> m_constantBuffer;
};

class PlayerObject : public Object
{
public:
	PlayerObject(Scene* root);
	void OnUpdate(GameTimer& gTimer) override;
private:
	void OnKeyboardInput(const GameTimer& gTimer);
};

class CameraObject : public Object
{
public:
	CameraObject(Scene* root, float radius);
	void OnUpdate(GameTimer& gTimer) override;
	void LateUpdate(GameTimer& gTimer) override;
	void OnMouseInput(WPARAM wParam, HWND hWnd);
private:
	int mLastPosX;
	int mLastPosY;
	float mTheta;
	float mPhi;
	float mRadius;
};

class TerrainObject : public Object
{
public:
	TerrainObject(Scene* root);
};

class TestObject : public Object
{
public:
	TestObject(Scene* root);
};

class TreeObject : public Object
{
public:
	TreeObject(Scene* root);
};

class TigerObject : public Object
{
public:
	TigerObject(Scene* root);
	void OnUpdate(GameTimer& gTimer) override;
private:
	void TigerBehavior(GameTimer& gTimer);
	void RandomVelocity(GameTimer& gTimer);
	float mTimer;
	XMFLOAT3 mTempVelocity;
};

class StoneObject : public Object
{
public:
	StoneObject(Scene* root);
};