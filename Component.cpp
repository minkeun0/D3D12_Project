#include "Component.h"
#include "Object.h"
#include "Scene.h"

Transform::Transform(XMVECTOR pos, XMVECTOR rot, XMVECTOR scale)
{
	XMStoreFloat3(&mScale, scale);
	XMStoreFloat3(&mRotation, rot);
	XMStoreFloat3(&mPosition, pos);
	//XMStoreFloat4(&mQuaternion, XMQuaternionNormalize(GetQuaternionFromRotation()));
	XMStoreFloat4x4(&mFinalM, GetTransformM());
}

XMVECTOR Transform::GetScale()
{
	return XMVectorSet(mScale.x, mScale.y, mScale.z, 0.0f);
}

XMVECTOR Transform::GetRotation()
{
	return XMVectorSet(mRotation.x, mRotation.y, mRotation.z, 0.0f);
}

XMVECTOR Transform::GetQuaternion()
{
	return  XMQuaternionNormalize(XMVectorSet(mQuaternion.x, mQuaternion.y, mQuaternion.z, mQuaternion.w));
}

XMVECTOR Transform::GetPosition()
{
	return XMVectorSet(mPosition.x, mPosition.y, mPosition.z, 1.0f);
}

XMMATRIX Transform::GetTranslateM()
{
	return XMMatrixTranslationFromVector(GetPosition());
}

XMMATRIX Transform::GetScaleM()
{
	return XMMatrixScalingFromVector(GetScale());
}

XMMATRIX Transform::GetRotationM()
{
	XMVECTOR rot = GetRotation();
	return XMMatrixRotationRollPitchYawFromVector(rot * XM_PI / 180);
}

XMMATRIX Transform::GetRotationQuaternionM()
{
	return XMMatrixRotationQuaternion(GetQuaternion());
}

XMMATRIX Transform::GetTransformM()
{
	return GetScaleM() * GetRotationM() * GetTranslateM();
}

XMMATRIX Transform::GetFinalM()
{
	return XMLoadFloat4x4(&mFinalM);
}

	XMVECTOR Transform::GetQuaternionFromRotation()
	{
		XMVECTOR q1 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(mRotation.x));
		XMVECTOR q2 = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(mRotation.y));
		XMVECTOR q3 = XMQuaternionRotationAxis(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), XMConvertToRadians(mRotation.z));
		return XMQuaternionMultiply(XMQuaternionMultiply(q3, q1), q2);
		//return XMQuaternionRotationRollPitchYaw(
		//	XMConvertToRadians(mRotation.x),
		//	XMConvertToRadians(mRotation.y),
		//	XMConvertToRadians(mRotation.z));
	}
	
	void Transform::SetPosition(XMVECTOR pos)
	{
		XMStoreFloat3(&mPosition, pos);
	}
	
	void Transform::SetRotation(XMVECTOR rot)
	{
		XMStoreFloat3(&mRotation, rot);
		XMStoreFloat4(&mQuaternion, GetQuaternionFromRotation());
	}
	
	void Transform::SetQuaternion(XMVECTOR qua)
	{
		XMStoreFloat4(&mQuaternion, qua);
	}
	
	void Transform::SetFinalM(XMMATRIX finalM)
	{
		XMStoreFloat4x4(&mFinalM, finalM);
	}

XMMATRIX AdjustTransform::GetTranslateM()
{
	return XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
}

AdjustTransform::AdjustTransform(XMVECTOR pos, XMVECTOR rot, XMVECTOR scale)
{
	XMStoreFloat3(&mScale, scale);
	XMStoreFloat3(&mRotation, rot);
	XMStoreFloat3(&mPosition, pos);
}

XMMATRIX AdjustTransform::GetScaleM()
{
	return XMMatrixScalingFromVector(XMLoadFloat3(&mScale));
}

XMMATRIX AdjustTransform::GetRotationM()
{
	XMVECTOR rot = XMLoadFloat3(&mRotation);
	return XMMatrixRotationRollPitchYawFromVector(rot * XM_PI / 180);
}

XMMATRIX AdjustTransform::GetTransformM()
{
	return GetScaleM() * GetRotationM() * GetTranslateM();
}

XMVECTOR Gravity::ProcessGravity(XMVECTOR pos, float deltaTime)
{
	float gForce = 60.0f + mG * mElapseTime * mElapseTime;
	if (mElapseTime == 0.0f) gForce = 0.0f;
	mVerticalSpeed -= gForce * deltaTime;
	mElapseTime += deltaTime;

	XMFLOAT3 newPos{};
	XMStoreFloat3(&newPos, pos);
	newPos.y += mVerticalSpeed * deltaTime;
	return XMLoadFloat3(&newPos);
}

void Gravity::ResetElapseTime()
{
	// ���� �ӵ��� ����̸� pos�� y ��Ұ� �������� �ǹ��Ѵ�.
	// �̶� �ٴڸ�� �浹�ϴ��� �������� �ʴ´�. 
	// ���� �ӵ��� ������ ���. ��, pos �� y��Ұ� �����ϴ� ��쿡�� �ٴڸ� �浹�� �����Ѵ�. 

	if (mVerticalSpeed <= 0.0f) {
		mElapseTime = 0.0f;
		mVerticalSpeed = 0.0f;
	}

}

float Gravity::GetElapseTime()
{
	return mElapseTime;
}

void Gravity::SetVerticalSpeed(float speed)
{
	mVerticalSpeed = speed;
}

Collider::Collider(XMFLOAT3&& center, XMFLOAT3&& extents, XMFLOAT4&& orientation) :
	mBaseOBB{ std::move(center), std::move(extents), std::move(orientation) }
{
}

void Collider::UpdateOBB(XMMATRIX M)
{
	mBaseOBB.Transform(mOBB, M);
	XMStoreFloat4(&mOBB.Orientation, XMQuaternionNormalize(XMLoadFloat4(&mOBB.Orientation)));
}

BoundingOrientedBox& Collider::GetOBB()
{
	return mOBB;
}

Animation::Animation(string initFileName) : mCurrentFileName{initFileName}
{
}

bool Animation::ResetAnim(string fileName, float time)
{
	if (fileName == mCurrentFileName) return false;
	mCurrentFileName = fileName;
	mAnimationTime = time;
	return true;
}
