#include <fstream>
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

	// 평면
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

void ResourceManager::CreateTerrain(const string& name, int maxHeight)
{
	ifstream in{ name };
	if (!in) throw;

	in.seekg(0, ios::end);
	int fileSize = in.tellg();
	in.seekg(0, ios::beg);

	vector<uint8_t> heightMap(fileSize);
	in.read(reinterpret_cast<char*>(heightMap.data()), fileSize);


	int width = sqrt(fileSize);
	int height = sqrt(fileSize);

	vector<float> heightData(width * height);
	for (int z = 0; z < height; ++z) {
		for (int x = 0; x < width; ++x) {
			heightData[z * width + x] = heightMap[(height - 1 - z) * width + x] / 255.f * maxHeight; // (height - 1 - z)는 왼쪽 아래를 원점(원래 원점은 왼쪽 위)으로 하기 위함이다.
			//heightData[z * width + x] = heightMap[z * width + x] / 255.f * maxHeight; // (height - 1 - z) 의 의미는 왼쪽 아래를 원점으로 하기 위함이다.

		}
	}

	vector<Vertex> vertices(width * height);
	for (int z = 0; z < height; ++z) {
		for (int x = 0; x < width; ++x) {
			// position
			float y = heightData[z * width + x];
			vertices[z * width + x].position.x = x;
			vertices[z * width + x].position.y = y;
			vertices[z * width + x].position.z = z;
			// position.
			
			// normal
			XMVECTOR left = XMVectorZero();
			if (x == 0)
				left = { (float)x, heightData[z * width + x], (float)z };
			else
				left = { x - 1.f, heightData[z * width + (x - 1)], (float)z };

			XMVECTOR right = XMVectorZero();
			if (x == width - 1)
				right = { (float)x, heightData[z * width + x], (float)z };
			else
				right = { x + 1.f, heightData[z * width + (x + 1)], (float)z };

			XMVECTOR up = XMVectorZero();
			if (z == height - 1)
				up = { (float)x, heightData[z * width + x], (float)z };
			else
				up = { (float)x, heightData[(z + 1) * width + x], z + 1.f };

			XMVECTOR down = XMVectorZero();
			if (z == 0)
				down = { (float)x, heightData[z * width + x], (float)z };
			else
				down = { (float)x, heightData[(z - 1) * width + x], z - 1.f };

			XMVECTOR normal = XMVector3Cross(up - down, right - left);
			normal = XMVector3Normalize(normal);
		
			vertices[z * width + x].normal.x = XMVectorGetX(normal);
			vertices[z * width + x].normal.y = XMVectorGetY(normal);
			vertices[z * width + x].normal.z = XMVectorGetZ(normal);
			// normal.
			
			// uv
			vertices[z * width + x].uv.x = x;
			vertices[z * width + x].uv.y = height - 1 - z;
			// uv.
		}
	}

	vector<uint32_t> indices((width - 1) * (height - 1) * 2 * 3);
	int i{ 0 };
	for (int z = 0; z < height - 1; ++z) {
		for (int x = 0; x < width - 1; ++x) {
			int topLeft = (z + 1) * width + x;
			int topRight = topLeft + 1;
			int bottomLeft = z * width + x;
			int bottomRight = bottomLeft + 1;

			indices[i++] = bottomLeft;
			indices[i++] = topLeft;
			indices[i++] = topRight;

			indices[i++] = bottomLeft;
			indices[i++] = topRight;
			indices[i++] = bottomRight;
		}
	}

	SubMeshData subData{};
	subData.vertexCountPerInstance = vertices.size();
	subData.indexCountPerInstance = indices.size();
	subData.startVertexLocation = mVertexBuffer.size();
	subData.startIndexLocation = mIndexBuffer.size();
	subData.baseVertexLocation = mVertexBuffer.size();
	mSubMeshData[name] = subData;

	mVertexBuffer.insert(mVertexBuffer.end(), vertices.begin(), vertices.end());
	//OutputDebugStringA(string{ "##### " + (to_string(mVertexBuffer.size()) + "\n") }.c_str());

	mIndexBuffer.insert(mIndexBuffer.end(), indices.begin(), indices.end());
}

vector<Vertex>& ResourceManager::GetVertexBuffer()
{
	return mVertexBuffer;
}

vector<uint32_t>& ResourceManager::GetIndexBuffer()
{
	return mIndexBuffer;
}

unordered_map<string, SubMeshData>& ResourceManager::GetSubMeshData()
{
	return mSubMeshData;
}

unordered_map<string, SkinnedData>& ResourceManager::GetAnimationData()
{
	return mAnimData;
}
