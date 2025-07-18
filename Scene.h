#pragma once
#include "stdafx.h"
#include "Object.h"
#include "ResourceManager.h"
#include <utility>
#include "Shadow.h"

class GameTimer;
class Framework;

class Scene
{
public:
    ~Scene();
    Scene() = default;
    Scene(Framework* parent, UINT width, UINT height, wstring name);
    void OnInit(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    void OnUpdate(GameTimer& gTimer);
    void CheckCollision();
    void LateUpdate(GameTimer& gTimer);
    void OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ePass pass);
    void OnResize(UINT width, UINT height);
    void OnDestroy();
    void OnKeyDown(UINT8 key);
    void OnKeyUp(UINT8 key);
    wstring GetSceneName() const;
    ResourceManager& GetResourceManager();
    void* GetConstantBufferMappedData();
    ID3D12DescriptorHeap* GetDescriptorHeap();
    UINT CalcConstantBufferByteSize(UINT byteSize);
    Framework* GetFramework();
    UINT GetNumOfTexture();
    void AddObj(Object* object);
    template<typename T> 
    T* GetObj() 
    {
        Object* temp = nullptr;
        for (Object* obj : m_objects) {
            temp = dynamic_cast<T*>(obj);
            if (temp) break;
        }
        return temp; 
    }
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>>& GetPSOs();
    void RenderObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    std::array<eKeyState, static_cast<size_t>(eKeyTable::SIZE)>& GetKeyBuffer();
private:
    void LoadMeshAnimationTexture();
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
    void BuildProjMatrix();
    void BuildObjects(ID3D12Device* device);
    void BuildShadow();
    void BuildShaders();
    void BuildInputElement();
    ComPtr<ID3DBlob> CompileShader(
        const std::wstring& fileName, const D3D_SHADER_MACRO* defines, const std::string& entryPoint, const std::string& target);
    void UpdateKeyBuffer();
private:
    Framework* m_parent = nullptr;
    wstring m_name;
    vector<Object*> m_objects;
    unique_ptr<ResourceManager> m_resourceManager;
    //
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<ID3D12RootSignature> m_rootSignature;
    //std::unordered_map<std::string, ComPtr<ID3D12RootSignature>> m_rootSignatures;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> m_PSOs;
    std::unordered_map<std::string, ComPtr<ID3DBlob>> m_shaders;
    //
    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    UINT m_cbvsrvuavDescriptorSize;
    //
    ComPtr<ID3D12Resource> m_vertexBuffer_default;
    ComPtr<ID3D12Resource> m_indexBuffer_default;
    ComPtr<ID3D12Resource> m_vertexBuffer_upload;
    ComPtr<ID3D12Resource> m_indexBuffer_upload;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
    //
    unordered_map<wstring, int> m_subTextureData;
    vector<wstring> m_DDSFileName;
    vector<ComPtr<ID3D12Resource>> m_textureBuffer_defaults;
    vector<ComPtr<ID3D12Resource>> m_textureBuffer_uploads;
    //
    ComPtr<ID3D12Resource> m_constantBuffer;
    void* m_mappedData = nullptr;
    //
    XMFLOAT4X4 m_proj;
    //
    unique_ptr<Shadow> m_shadow = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputElement;

    std::array<eKeyState, static_cast<size_t>(eKeyTable::SIZE)> mKeyBuffer = {};

    std::unordered_map <eBehavior, std::unordered_map<eEvent, eBehavior>> mPlayerTransitions = {
        {eBehavior::Idle, {{eEvent::MoveKeyPressed, eBehavior::Walk}}},
        {eBehavior::Walk, {{eEvent::ShiftKeyPressed, eBehavior::Run}, {eEvent::MoveKeyReleased, eBehavior::Idle}}},
        {eBehavior::Run, {{eEvent::ShiftKeyReleased, eBehavior::Walk}, {eEvent::MoveKeyReleased, eBehavior::Idle}}},
    };
};
