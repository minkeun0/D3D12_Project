#pragma once
#include "stdafx.h"
class FbxBase
{
public:
	FbxBase();
	~FbxBase();

	void ImportFbxToScene(ptr<const char> filename);
private:
	ptr<FbxManager> m_fbxManager;
	ptr<FbxIOSettings> m_fbxIOS;
	ptr<FbxScene> m_fbxScene;
	ptr<FbxImporter> m_fbxImporter;
};

