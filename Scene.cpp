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
    LoadPipeline(device);
}

// Load the rendering pipeline dependencies.
void Scene::LoadPipeline(ID3D12Device* device)
{
    // Create the command allocator.
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

    // Create the command list.
    ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());
}

// Load the sample assets.
void Scene::LoadAssets(ID3D12Device* device)
{
    // Create the command allocator.
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

    // Create the command list.
    ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());
}

// Update frame-based values.
void Scene::OnUpdate()
{
}

// Render the scene.
void Scene::OnRender(ID3D12Resource* renderTarget, ID3D12DescriptorHeap* rtvHeap, UINT rtvDescriptorSize, UINT frameIndex)
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList(renderTarget, rtvHeap, rtvDescriptorSize, frameIndex);
}

void Scene::OnDestroy()
{

}

std::wstring Scene::GetSceneName() const
{
    return m_name;
}

ID3D12GraphicsCommandList* Scene::GetCommandList() const
{
    return m_commandList.Get();
}

void Scene::PopulateCommandList(ID3D12Resource* renderTarget, ID3D12DescriptorHeap* rtvHeap, UINT rtvDescriptorSize, UINT frameIndex)
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(m_commandAllocator->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

    // Indicate that the back buffer will be used as a render target.
    m_commandList->ResourceBarrier(1, 
        &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);

    // Record commands.
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // Indicate that the back buffer will now be used to present.
    m_commandList->ResourceBarrier(1, 
        &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    ThrowIfFailed(m_commandList->Close());
}