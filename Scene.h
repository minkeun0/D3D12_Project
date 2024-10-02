#pragma once
#include "stdafx.h"
#include "Object.h"

using Microsoft::WRL::ComPtr;

class GameTimer;

class Scene
{
public:
    Scene() = default;
    Scene(UINT width, UINT height, std::wstring name);

    virtual void OnInit(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    virtual void OnUpdate(GameTimer& gTimer);
    virtual void OnRender(ID3D12GraphicsCommandList* commandList);
    virtual void OnResize(UINT width, UINT height);
    virtual void OnDestroy();

    void SetState(ID3D12GraphicsCommandList* commandList);
    void SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList);

    std::wstring GetSceneName() const;
private:
    std::wstring m_name;

    std::unordered_map<std::wstring, std::unique_ptr<Object>> m_Object;

    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    // App resources.
    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    UINT m_cbvsrvuavDescriptorSize;
    //
    vector<Vertex> m_vertexData;
    vector<uint16_t> m_indexData;
    UINT m_vertexDataSize;
    UINT m_indexDataSize;
    ComPtr<ID3D12Resource> m_vertexBuffer_default;
    ComPtr<ID3D12Resource> m_indexBuffer_default;
    ComPtr<ID3D12Resource> m_vertexBuffer_upload;
    ComPtr<ID3D12Resource> m_indexBuffer_upload;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
    //
    ComPtr<ID3D12Resource> m_textureBuffer_default;
    ComPtr<ID3D12Resource> m_textureBuffer_upload;
    //
    ComPtr<ID3D12Resource> m_constantBuffer;
    UINT8* m_MappedData;
    //
    XMFLOAT4X4 m_proj;
    //
    void BuildObjects(ID3D12Device* device);
    void BuildRootSignature(ID3D12Device* device);
    void BuildPSO(ID3D12Device* device);
    void BuildVertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    void BuildIndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    void BuildVertexBufferView();
    void BuildIndexBufferView();
    void BuildConstantBuffer(ID3D12Device* device);
    void BuildConstantBufferView(ID3D12Device* device);
    void BuildTextureBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    void BuildTextureBufferView(ID3D12Device* device);
    void BuildDescriptorHeap(ID3D12Device* device);
    UINT CalcConstantBufferByteSize(UINT byteSize);
    void BuildMesh();
    void BuildProjMatrix();
};
