#pragma once
#include <DirectXCollision.h>
#include "stdafx.h"

class Scene;

class Shadow
{
public:
private:
	Scene* mParent;
	BoundingSphere mSceneBound;
	XMFLOAT3 mLightDirection;
	XMFLOAT4X4 mViewMatrix;
	XMFLOAT4X4 mProjMatrix;
	XMFLOAT4X4 mShadowTransform;
};

