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
};

