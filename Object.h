#pragma once
#include "stdafx.h"
#include "Component.h"

class GameTimer;
class Scene;

class Object
{
public:
	virtual ~Object();
	Object(Scene* scene, uint32_t id, uint32_t parentId = -1);
	virtual void OnUpdate(GameTimer& gTimer);
	virtual void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration);
	virtual void LateUpdate(GameTimer& gTimer);
	virtual void OnRender(ID3D12Device* device, ID3D12GraphicsCommandList * commandList);
	void ProcessAnimation(GameTimer& gTimer);
	void BuildConstantBuffer(ID3D12Device* device);
	void AddComponent(Component* component);
	Scene* GetScene() { return m_scene; }
	uint32_t GetId();
	bool GetValid();
	void Delete();

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
	Scene* m_scene = nullptr;
	uint32_t m_id = -1;
	uint32_t m_parent_id = -1;
	bool m_valid = true;
	vector<Component*> m_components;

	// 오브젝트 마다 독립적인 CB
	UINT8* m_mappedData = nullptr;
	ComPtr<ID3D12Resource> m_constantBuffer;
};

class PlayerObject : public Object
{
public:
	using Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
	int GetRiceCakeCount();
	int GetLifeCount();
private:
	void ProcessInput(const GameTimer& gTimer);
	void ChangeState(string fileName);
	void MoveAndRotate(float deltaTime);
	void Idle();
	void Walk();
	void Run();
	void Jump();
	void Attack();
	void Throw();
	void Fire();
	void Hit();
	void Dead();
	void TimeOut();
	void CalcTime(float deltaTime);
	float mSpeed = 20.0f;
	float mElapseTime = 0.0f;
	float mAttackTime = 0.0f;
	bool mIsFired = false;
	bool mIsHitted = false;
	bool mIsJumpping = false;
	int mLife = 3;
	XMFLOAT3 mCameraLookDir{};
	int mRiceCake = 0;
	bool mFocusMode = false;
};

class CameraObject : public Object
{
public:
	using Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void LateUpdate(GameTimer& gTimer) override;
private:
	void ProcessInput();
	void MouseMove();
	float mTheta = XMConvertToRadians(-90.0f);
	float mPhi = XMConvertToRadians(60.0f);
	float mRadius = 70.0f;
	float mFocusModeRadius = 15.0f;
	bool mFocusMode = false;
	float mDeltaX = 0.0f;
	float mDeltaY = 0.0f;
};

class TerrainObject : public Object
{
public:
	using Object::Object;
};

class TestObject : public Object
{
public:
	using Object::Object;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
};

class TreeObject : public Object
{
public:
	using Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
	void LateUpdate(GameTimer& gTimer) override;

private:
	unsigned char mCollisionByPlayerAttack = (unsigned char)0x00;
};

class TigerObject : public Object
{
public:
	using Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
	int GetLife();
private:
	void TigerBehavior(GameTimer& gTimer);
	void ChangeState(string fileName);
	void Walk();
	void Run();
	void Attack();
	void TimeOut();
	void Fire();
	void Hit();
	void HitByRiceCake();
	void Dead();
	void CalcTime(float deltaTime);
	void CreateLeather();
	float mWalkSpeed = 25.0f;
	float mRunSpeed = 45.0f;
	float mElapseTime = 0.0f;
	float mAttackTime = 0.0f;
	float mSearchTime = 0.0f;
	bool mIsFired = false;
	bool mIsHitted = false;
	int mLife = 3;
};

class TigerAttackObject : public Object
{
public:
	using Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
private:
	float mElapseTime = 0.0f;
};

class PlayerAttackObject : public Object
{
public:
	using Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
private:
	float mElapseTime = 0.0f;
};

class TigerMockup : public Object
{
public:
	using Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
private:
	float mSearchTime = 0.0f;
	float mWalkSpeed = 20.0f;
};

class TigerLeather : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
private:
};

class SisterObject : public Object
{
public:
	Object::Object;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;

private:
	bool mIsQuadAble = false;
};

class GodObject : public Object
{
public:
	Object::Object;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
private:
};

class MovePlatformObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
private:
	float mSpeed = 30.0f;
	float mElapseTime = 0.0f;
};

class RotPlatformObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
private:
	float mSpeed = 30.0f;
};

class PuzzlePlatformObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
private:
	float mSpeed = 30.0f;
	float mMin = 5.0f;
	float mMax = 150.0f;
};

class AxeObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
private:
};

class RiceCakeObject : public Object
{
public:
	Object::Object;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
};

class RiceCakeProjectileObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
	void SetDir(XMVECTOR dir);
	
private:
	XMFLOAT3 mDir{};
	float mSpeed = 200.0f;
};

class GoToBaseObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;

private:
	float mElapseTime = 0.0f;
};

class SisterQuadObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
};

class TitleQuadObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
};

class EndQuadObject : public Object
{
public:
	Object::Object;
};


class LifeQuadObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
};

class BoyIconQuadObject : public Object
{
public:
	Object::Object;
};

class RiceCakeQuadObject : public Object
{
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
};

class TigerLeatherQuadObject : public Object
{
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
};

class CrossHairQuadObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
};

class PuzzleCellObject : public Object
{
public:
	Object::Object;
	void OnUpdate(GameTimer& gTimer) override;
	void OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration) override;
	int GetStatus();
	void SetStatus(int value);
private:
	int mStatus = 0;
};

class PuzzleFrameObject : public Object
{
public:
	PuzzleFrameObject(Scene* scene, uint32_t id, uint32_t parentId = -1);
	void OnUpdate(GameTimer& gTimer) override;
	bool AllCellMatch();
private:
	PuzzleCellObject* mCells[3][3] = {};
};

class PuzzleQuestObject : public Object
{
public:
	PuzzleQuestObject(Scene* scene, uint32_t id, uint32_t parentId = -1);
private:
};

class GrassGroupObject : public Object
{
public:
	GrassGroupObject(Scene* scene, uint32_t id, uint32_t parentId = -1);
	void RandomRot();
};