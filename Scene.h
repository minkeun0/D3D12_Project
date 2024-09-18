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

#pragma once

#include "DXSample.h"
// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;

class Scene
{
public:
    Scene() = default;
    Scene(UINT width, UINT height, std::wstring name);

    virtual void OnInit(ID3D12Device* device);
    virtual void OnUpdate();
    virtual void OnRender(ID3D12Resource* renderTarget, ID3D12DescriptorHeap* rtvHeap, UINT rtvDescriptorSize, UINT frameIndex);
    virtual void OnDestroy();

    std::wstring GetSceneName() const;
    ID3D12GraphicsCommandList* GetCommandList() const;
private:
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
    };

    std::wstring m_name;

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;

    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    // App resources.
    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    void LoadPipeline(ID3D12Device* device);
    void LoadAssets(ID3D12Device* device);
    void PopulateCommandList(ID3D12Resource* renderTarget, ID3D12DescriptorHeap* rtvHeap, UINT rtvDescriptorSize, UINT frameIndex);

};
