#pragma once
#include <variant>
#include <DirectXCollision.h>
#include "stdafx.h"
#include "Info.h"
#include "FbxExtractor.h"
#include <queue>

class Object;

struct Component // 객체로 만들지 않는 클래스
{
	Component() = default;
	virtual ~Component() = default;
};

class Transform : public Component
{
public:
	//Transform(XMFLOAT3&& pos, XMFLOAT3&& rot = { 0.0f, 0.0f, 0.0f }, XMFLOAT3&& scale = { 1.0f, 1.0f, 1.0f });
	Transform(XMVECTOR pos, XMVECTOR rot = { 0.0f, 0.0f, 0.0f, 0.0f }, XMVECTOR scale = { 1.0f, 1.0f, 1.0f, 0.0f });
	XMVECTOR GetScale();
	XMVECTOR GetRotation();
	XMVECTOR GetQuaternion();
	XMVECTOR GetPosition();
	XMMATRIX GetTranslateM();
	XMMATRIX GetScaleM();
	XMMATRIX GetRotationM();
	XMMATRIX GetRotationQuaternionM();
	XMMATRIX GetTransformM();
	XMMATRIX GetFinalM();
	void SetPosition(XMVECTOR pos);
	void SetRotation(XMVECTOR rot);
	void SetQuaternion(XMVECTOR qua);
	void SetFinalM(XMMATRIX finalM);
private:
	XMVECTOR GetQuaternionFromRotation();
	XMFLOAT3 mScale{ 1.0f, 1.0f, 1.0f };
	XMFLOAT3 mRotation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT4 mQuaternion{ 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT3 mPosition{ 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 mFinalM{
	1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f };
};

struct Mesh : public Component
{ 
	Mesh() = default;
	Mesh(SubMeshData& subMeshData) : mSubMeshData{ subMeshData } {}
	SubMeshData mSubMeshData;
};

struct Texture : public Component
{
	Texture() = default;
	Texture(int descriptorStartIndex) : mDescriptorStartIndex{descriptorStartIndex} {}
	int mDescriptorStartIndex;
};

struct Animation : public Component
{
	Animation() = default;
	Animation(unordered_map<string, SkinnedData>& animData) : mAnimData{ &animData }, mAnimationTime{ 0.f }, mSleepTime{ 0.f }, mCurrentFileName{} {}
	unordered_map<string, SkinnedData>* mAnimData;
	float mAnimationTime;
	float mSleepTime;
	string mCurrentFileName;
};

struct Position : public Component
{
	Position() = default;
	Position(float x, float y, float z) : mPosition{ x, y, z} {}
	XMFLOAT3 mPosition{};
};

struct Velocity : public Component
{
	Velocity() = default;
	Velocity(float x, float y, float z) : mVelocity{ x, y, z} {}
	XMFLOAT3 mVelocity{};
};

struct Rotation : public Component
{
	Rotation() = default;
	Rotation(float x, float y, float z, float w) : mRotation{ x, y, z} {}
	XMFLOAT3 mRotation{};
};


struct Scale : public Component
{
	Scale() = default;
	Scale(float x, float y, float z) : mScale{ x, y, z} {}
	XMFLOAT3 mScale;
};

struct Collider : public Component
{
	Collider() = default;
	Collider(float centerX, float centerY, float centerZ, float extentsX, float extentsY, float extentsZ) :
		mAABB{ XMFLOAT3{centerX, centerY, centerZ}, XMFLOAT3{extentsX, extentsY, extentsZ} },
		mLocalAABB{ XMFLOAT3{centerX, centerY, centerZ}, XMFLOAT3{extentsX, extentsY, extentsZ} } 
	{}
	bool FindCollisionObj(Object* objAddress) {
		auto it = mCollisionStates.find(objAddress);
		return it != mCollisionStates.end();
	}
	BoundingBox mAABB;
	BoundingBox mLocalAABB;
	unordered_map<Object*, CollisionState> mCollisionStates;
};