#pragma once
#include "stdafx.h"
#include "Object.h"

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
    virtual void OnRender(ID3D12GraphicsCommandList* commandList);
    virtual void OnDestroy();

    void SetState(ID3D12GraphicsCommandList* commandList);
    void SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList);
    std::wstring GetSceneName() const;
private:

    struct SceneConstantBuffer
    {
        XMFLOAT4 position;
    };


    std::wstring m_name;
    std::unordered_map<std::wstring, std::unique_ptr<Object>> m_Object;

    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;


    // App resources.
    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    ComPtr<ID3D12Resource> m_constantBuffer;
    ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
    UINT8* m_MappedData;
    SceneConstantBuffer m_constantBufferData;


    void BuildObjects(ID3D12Device* device);
    void BuildRootSignature(ID3D12Device* device);
    void BuildPSO(ID3D12Device* device);
    void BuildVertexBuffer(ID3D12Device* device);
    void BuidCBVHeap(ID3D12Device* device);
    void BuildConstantBuffer(ID3D12Device* device);
    UINT CalcConstantBufferByteSize(UINT byteSize);
};
