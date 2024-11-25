#include "ResourceManager.h"

ResourceManager::ResourceManager() : mFbxExtractor{ nullptr }, mVertexBuffer{}
{
	mFbxExtractor = make_unique<FbxExtractor>();
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::LoadFbx(const string& fileName, bool onlyAnimation, bool zUp)
{
	mFbxExtractor->ImportFbxFile(fileName, onlyAnimation, zUp);
	
	mFbxExtractor->ExtractDataFromFbx();

	if (onlyAnimation == false) {
		vector<Vertex>& vertexData = mFbxExtractor->GetVertices();
		SubMeshData subData{};
		subData.vertexCountPerInstance = vertexData.size();
		subData.startVertexLocation = mVertexBuffer.size();
		mSubMeshData[fileName] = subData;
		mVertexBuffer.insert(mVertexBuffer.end(), vertexData.begin(), vertexData.end());
		OutputDebugStringA(string{ "##### " + (to_string(mVertexBuffer.size()) + "\n") }.c_str());
	}

	SkinnedData animData;
	animData.Set(mFbxExtractor->GetBoneHierarchyIndex(), mFbxExtractor->GetOffsetMatrix(), mFbxExtractor->GetAnimation());
	mAnimData.emplace(fileName ,animData);


	mFbxExtractor->ResetAndClear();
}

void ResourceManager::CreatePlane(const string& name, float size)
{
	float halfSize = size / 2;
	float wrap = 100;
	vector<Vertex> vertexData;

	// Æò¸é
	{
		vertexData.push_back(Vertex{ {-halfSize, 0, halfSize},{0,1,0},{0,0} });
		vertexData.push_back(Vertex{ {halfSize, 0, halfSize},{0,1,0},{wrap,0} });
		vertexData.push_back(Vertex{ {-halfSize, 0, -halfSize},{0,1,0},{0,wrap} });
		vertexData.push_back(Vertex{ {-halfSize, 0, -halfSize},{0,1,0},{0,wrap} });
		vertexData.push_back(Vertex{ {halfSize, 0, halfSize},{0,1,0},{wrap,0} });
		vertexData.push_back(Vertex{ {halfSize, 0, -halfSize},{0,1,0},{wrap,wrap} });
	}

	SubMeshData subData{};
	subData.vertexCountPerInstance = vertexData.size();
	subData.startVertexLocation = mVertexBuffer.size();
	mSubMeshData[name] = subData;

	mVertexBuffer.insert(mVertexBuffer.end(), vertexData.begin(), vertexData.end());
	OutputDebugStringA(string{ "##### " + (to_string(mVertexBuffer.size()) + "\n") }.c_str());

}

Vertex* ResourceManager::GetVertexBuffer()
{
	return mVertexBuffer.data();
}

size_t ResourceManager::GetVertexBufferSize()
{
	return mVertexBuffer.size();
}

SubMeshData& ResourceManager::GetSubMeshData(const string& fileName)
{
	return mSubMeshData.at(fileName);
}

unordered_map<string, SkinnedData>& ResourceManager::GetAnimationData()
{
	return mAnimData;
}
