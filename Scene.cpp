//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "stdafx.h"
#include "Scene.h"

Scene::Scene(UINT width, UINT height, std::wstring name) :
    m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
    m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
    m_name(name)
{
}

void Scene::OnInit(ID3D12Device* device)
{
}

// Load the sample assets.
void Scene::LoadAssets(ID3D12Device* device)
{
}

// Update frame-based values.
void Scene::OnUpdate()
{
}

// Render the scene.
void Scene::OnRender()
{
}

void Scene::OnDestroy()
{

}

std::wstring Scene::GetSceneName() const
{
    return m_name;
}

ID3D12PipelineState* Scene::GetPSO() const
{
    return m_pipelineState.Get();
}
