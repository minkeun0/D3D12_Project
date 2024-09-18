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

Scene::Scene(std::wstring name) :
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
