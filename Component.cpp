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
