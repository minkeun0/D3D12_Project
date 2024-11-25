#pragma once
#include "stdafx.h"
#include "FbxExtractor.h"
#include "Info.h"

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();
	void LoadFbx(const string& fileName, bool onlyAnimation, bool zUp);
	void CreatePlane(const string& name, float size);
	Vertex* GetVertexBuffer();
	size_t GetVertexBufferSize();
	SubMeshData& GetSubMeshData(const string& fileName);
	unordered_map<string, SkinnedData>& GetAnimationData();
private:

	unique_ptr<FbxExtractor> mFbxExtractor;
	vector<Vertex> mVertexBuffer;
	unordered_map<string, SubMeshData> mSubMeshData;
	unordered_map<string, SkinnedData> mAnimData;
};

