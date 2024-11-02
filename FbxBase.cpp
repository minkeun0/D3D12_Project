#include "FbxBase.h"
#include "DXSampleHelper.h"

FbxBase::FbxBase() : m_fbxManager{ nullptr }, m_fbxIOS{ nullptr }, m_fbxScene{ nullptr }, m_fbxImporter{ nullptr }
{
	ThrowIfFailed(m_fbxManager = FbxManager::Create());
	ThrowIfFailed(m_fbxIOS = FbxIOSettings::Create(m_fbxManager, IOSROOT));
	m_fbxManager->SetIOSettings(m_fbxIOS);

}

FbxBase::~FbxBase()
{
	m_fbxManager->Destroy();
}

void FbxBase::ImportFbxToScene(ptr<const char> filename)
{
	ThrowIfFailed(m_fbxImporter = FbxImporter::Create(m_fbxManager, ""));
	ThrowIfFailed(m_fbxImporter->Initialize(filename, -1, m_fbxManager->GetIOSettings()));
	ThrowIfFailed(m_fbxScene = FbxScene::Create(m_fbxManager, "baseScene"));
	ThrowIfFailed(m_fbxImporter->Import(m_fbxScene));
	m_fbxImporter->Destroy();
}
