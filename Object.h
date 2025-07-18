#pragma once
#include "stdafx.h"
#include "Component.h"
#include <variant>

class GameTimer;
class Scene;
class Object
{
public:
	virtual ~Object();
	Object() = default;
	Object(Scene* root);

	//virtual void OnInit(ID3D12Device* device);
	virtual void OnUpdate(GameTimer& gTimer) = 0;
	virtual void LateUpdate(GameTimer& gTimer) = 0;
	virtual void OnRender(ID3D12Device* device, ID3D12GraphicsCommandList * commandList);
	//virtual void OnDestroy();

	void BuildConstantBuffer(ID3D12Device* device);

	void AddComponent(Component* component);

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
	Scene* GetParent() { return m_parent; }

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
	PlayerObject() = default;
	PlayerObject(Scene* root);
	void OnUpdate(GameTimer& gTimer) override;
	void LateUpdate(GameTimer& gTimer) override;
	void OnKeyboardInput(const GameTimer& gTimer);
private:
	XMMATRIX mRotation;
};

class CameraObject : public Object
{
public:
	CameraObject() = default;
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
	TerrainObject() = default;
	TerrainObject(Scene* root);
	void OnUpdate(GameTimer& gTimer) override;
	void LateUpdate(GameTimer& gTimer) override;
};

class TestObject : public Object
{
public:
	TestObject() = default;
	TestObject(Scene* root);
	void OnUpdate(GameTimer& gTimer) override;
	void LateUpdate(GameTimer& gTimer) override;
};

class TreeObject : public Object
{
public:
	TreeObject() = default;
	TreeObject(Scene* root);
	void OnUpdate(GameTimer& gTimer) override;
	void LateUpdate(GameTimer& gTimer) override;
};

class TigerObject : public Object
{
public:
	TigerObject() = default;
	TigerObject(Scene* root);
	void OnUpdate(GameTimer& gTimer) override;
	void LateUpdate(GameTimer& gTimer) override;
	void TigerBehavior(GameTimer& gTimer);
	void RandomVelocity(GameTimer& gTimer);
private:
	XMMATRIX mRotation;
	float mTimer;
	XMFLOAT3 mTempVelocity;
};

class StoneObject : public Object
{
public:
	StoneObject() = default;
	StoneObject(Scene* root);
	void OnUpdate(GameTimer& gTimer) override;
	void LateUpdate(GameTimer& gTimer) override;
};