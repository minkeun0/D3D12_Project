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
	XMFLOAT3 mLightDirection = {0.3f , 1.f, 0.f};

};

