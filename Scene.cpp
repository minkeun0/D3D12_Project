#include "Scene.h"
#include "DXSampleHelper.h"
#include "GameTimer.h"

Scene::Scene(UINT width, UINT height, std::wstring name) :
    m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
    m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
    m_name(name),
    m_mappedData(nullptr)
{
    BuildProjMatrix();
    m_resourceManager = make_unique<ResourceManager>();
    m_resourceManager->CreatePlane("Plane", 5000);
    m_resourceManager->LoadFbx("Gunship.fbx");
    m_resourceManager->LoadFbx("1P(boy).fbx");
    m_resourceManager->LoadFbx("god.fbx");
    m_resourceManager->LoadFbx("FlyerPlayership.fbx");
    m_resourceManager->LoadFbx("sister.fbx");

    int i = 0;
    m_DDSFileName.push_back(L"./Textures/boy.dds");
    m_subTextureData.insert({ L"boy", i++ });
    m_DDSFileName.push_back(L"./Textures/bricks3.dds");
    m_subTextureData.insert({ L"bricks3", i++ });
    m_DDSFileName.push_back(L"./Textures/checkboard.dds");
    m_subTextureData.insert({ L"checkboard", i++ });
    m_DDSFileName.push_back(L"./Textures/grass.dds");
    m_subTextureData.insert({ L"grass", i++ });
    m_DDSFileName.push_back(L"./Textures/tile.dds");
    m_subTextureData.insert({ L"tile", i++ });
    m_DDSFileName.push_back(L"./Textures/WireFence.dds");
    m_subTextureData.insert({ L"WireFence", i++ });
    m_DDSFileName.push_back(L"./Textures/god.dds");
    m_subTextureData.insert({ L"god", i++ });
    m_DDSFileName.push_back(L"./Textures/Gunship.dds");
    m_subTextureData.insert({ L"Gunship", i++ });
    m_DDSFileName.push_back(L"./Textures/sister.dds");
    m_subTextureData.insert({ L"sister", i++ });
}

void Scene::OnInit(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    BuildObjects(device);
    BuildRootSignature(device);
    BuildPSO(device);
    BuildVertexBuffer(device, commandList);
    //BuildIndexBuffer(device, commandList);
    BuildTextureBuffer(device, commandList);
    BuildConstantBuffer(device);
    BuildDescriptorHeap(device);
    BuildVertexBufferView();
    //BuildIndexBufferView();
    BuildConstantBufferView(device);
    BuildTextureBufferView(device);
}

void Scene::BuildObjects(ID3D12Device* device)
{
    AddObj(L"PlayerObject", PlayerObject{ this });
    PlayerObject& player = GetObj<PlayerObject>(L"PlayerObject");
    player.AddComponent(Position{ 0.f, 12.f, -60.f, 1.f, &player });
    player.AddComponent(Velocity{ 0.f, 0.f, 0.f, 0.f, &player });
    player.AddComponent(Rotation{ -90.0f, 0.0f, 0.0f, 0.0f, &player });
    player.AddComponent(Rotate{ 0.0f, 10.0f, 0.0f, 0.0f, &player });
    player.AddComponent(Scale{ 0.1f, &player });
    player.AddComponent(World{ &player });
    player.AddComponent(Mesh{ GetResourceManager().GetSubMeshData("1P(boy).fbx"), &player });
    player.AddComponent(Texture{ m_subTextureData.at(L"boy"), &player});

    AddObj(L"CameraObject", CameraObject{30.f, this });
    CameraObject& camera = GetObj<CameraObject>(L"CameraObject");
    camera.AddComponent(Position{ 0.f, 0.f, 0.f, 0.f, &camera });

    AddObj(L"PlaneObject", TestObject{ this });
    TestObject& plane = GetObj<TestObject>(L"PlaneObject");
    plane.AddComponent(Position{ 0.f, 0.f, 0.f, 1.f, &plane });
    plane.AddComponent(Velocity{ 0.f, 0.f, 0.f, 0.f, &plane });
    plane.AddComponent(Rotation{ 0.0f, 0.0f, 0.0f, 0.0f, &plane });
    plane.AddComponent(Rotate{ 0.0f, 0.0f, 0.0f, 0.0f, &plane });
    plane.AddComponent(Scale{ 1.f, &plane });
    plane.AddComponent(World{ &plane });
    plane.AddComponent(Mesh{ GetResourceManager().GetSubMeshData("Plane") , &plane });
    plane.AddComponent(Texture{ m_subTextureData.at(L"tile"), &plane });

    AddObj(L"TestObject", TestObject{ this });
    TestObject& test = GetObj<TestObject>(L"TestObject");
    test.AddComponent(Position{ 50.f, 20.f, 0.f, 1.f, &test });
    test.AddComponent(Velocity{ 0.f, 0.f, 0.f, 0.f, &test });
    test.AddComponent(Rotation{ -90.0f, 0.0f, 0.0f, 0.0f, &test });
    test.AddComponent(Rotate{ 0.0f, 10.0f, 0.0f, 0.0f, &test });
    test.AddComponent(Scale{ 0.25f, &test });
    test.AddComponent(World{ &test });
    test.AddComponent(Mesh{ GetResourceManager().GetSubMeshData("sister.fbx") , &test });
    test.AddComponent(Texture{ m_subTextureData.at(L"sister"), &test });

    AddObj(L"TestObject1", TestObject{ this });
    TestObject& test1 = GetObj<TestObject>(L"TestObject1");
    test1.AddComponent(Position{ -20.f, 15.f, 0.f, 1.f, &test1 });
    test1.AddComponent(Velocity{ 0.f, 0.f, 0.f, 0.f, &test1 });
    test1.AddComponent(Rotation{ -90.0f, 0.0f, 0.0f, 0.0f, &test1});
    test1.AddComponent(Rotate{ 0.0f, 10.0f, 0.0f, 0.0f, &test1 });
    test1.AddComponent(Scale{ 0.25f, &test1 });
    test1.AddComponent(World{ &test1 });
    test1.AddComponent(Mesh{ GetResourceManager().GetSubMeshData("god.fbx"), &test1 });
    test1.AddComponent(Texture{ m_subTextureData.at(L"god"), &test1 });

    AddObj(L"TestObject2", TestObject{ this });
    TestObject& test2 = GetObj<TestObject>(L"TestObject2");
    test2.AddComponent(Position{ 0.f, 0.f, 100.f, 1.f, &test2 });
    test2.AddComponent(Velocity{ 0.f, 0.f, 0.f, 0.f, &test2 });
    test2.AddComponent(Rotation{ 0.0f, 0.0f, 0.0f, 0.0f, &test2 });
    test2.AddComponent(Rotate{ 0.0f, 10.0f, 0.0f, 0.0f, &test2 });
    test2.AddComponent(Scale{ 5.f, &test2 });
    test2.AddComponent(World{ &test2 });
    test2.AddComponent(Mesh{ GetResourceManager().GetSubMeshData("Gunship.fbx") , &test2 });
    test2.AddComponent(Texture{ m_subTextureData.at(L"Gunship"), &test2 });

}

void Scene::BuildRootSignature(ID3D12Device* device)
{
    // Create a root signature consisting of a descriptor table with a single CBV.
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};

    // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    CD3DX12_DESCRIPTOR_RANGE1 ranges[2] = {};
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

    CD3DX12_ROOT_PARAMETER1 rootParameters[3] = {};
    rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[2].InitAsConstants(16, 0, 0);

    D3D12_STATIC_SAMPLER_DESC sampler{};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // Allow input layout and deny uneccessary access to certain pipeline stages.
    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
    ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
}

void Scene::BuildPSO(ID3D12Device* device)
{
    // Create the pipeline state, which includes compiling and loading shaders.
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ThrowIfFailed(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
    ThrowIfFailed(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

    // Define the vertex input layout.
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState)));
}

void Scene::BuildVertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    const UINT vertexBufferSize = m_resourceManager->GetVertexBufferSize() * sizeof(Vertex);
    // Create the vertex buffer.
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(m_vertexBuffer_default.GetAddressOf())));

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_vertexBuffer_upload.GetAddressOf())));

    D3D12_SUBRESOURCE_DATA subResourceData{};
    subResourceData.pData = m_resourceManager->GetVertexBuffer();
    subResourceData.RowPitch = vertexBufferSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer_default.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
    UpdateSubresources(commandList, m_vertexBuffer_default.Get(), m_vertexBuffer_upload.Get(), 0, 0, 1, &subResourceData);
    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer_default.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
}

//void Scene::BuildIndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
//{
//    // Create the index buffer.
//    const UINT indexBufferSize = m_meshManager->GetindexBufferByteSize();
//    ThrowIfFailed(device->CreateCommittedResource(
//        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
//        D3D12_HEAP_FLAG_NONE,
//        &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
//        D3D12_RESOURCE_STATE_COMMON,
//        nullptr,
//        IID_PPV_ARGS(m_indexBuffer_default.GetAddressOf())));
//
//    ThrowIfFailed(device->CreateCommittedResource(
//        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
//        D3D12_HEAP_FLAG_NONE,
//        &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
//        D3D12_RESOURCE_STATE_GENERIC_READ,
//        nullptr,
//        IID_PPV_ARGS(m_indexBuffer_upload.GetAddressOf())));
//
//    D3D12_SUBRESOURCE_DATA subResourceData = {};
//    subResourceData.pData = m_meshManager->GetIndexData();
//    subResourceData.RowPitch = indexBufferSize;
//    subResourceData.SlicePitch = subResourceData.RowPitch;
//
//    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer_default.Get(),
//        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
//    UpdateSubresources(commandList, m_indexBuffer_default.Get(), m_indexBuffer_upload.Get(), 0, 0, 1, &subResourceData);
//    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer_default.Get(),
//        D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
//}

void Scene::BuildVertexBufferView()
{
    // Initialize the vertex buffer view.
    m_vertexBufferView.BufferLocation = m_vertexBuffer_default->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = m_resourceManager->GetVertexBufferSize() * sizeof(Vertex);
}

//void Scene::BuildIndexBufferView()
//{
//    m_indexBufferView.BufferLocation = m_indexBuffer_default->GetGPUVirtualAddress();
//    m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
//    m_indexBufferView.SizeInBytes = m_meshManager->GetindexBufferByteSize();
//}

void Scene::BuildDescriptorHeap(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC HeapDesc = {};
    HeapDesc.NumDescriptors = static_cast<UINT>(1 + m_DDSFileName.size());
    HeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    HeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    ThrowIfFailed(device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(m_descriptorHeap.GetAddressOf())));

    m_cbvsrvuavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Scene::BuildConstantBuffer(ID3D12Device* device)
{
    const UINT constantBufferSize = CalcConstantBufferByteSize(sizeof(CommonCB));    // CB size is required to be 256-byte aligned.

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer)));

    // Map and initialize the constant buffer. We don't unmap this until the
    // app closes. Keeping things mapped for the lifetime of the resource is okay.
    CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
    ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData)));
}

void Scene::BuildConstantBufferView(ID3D12Device* device)
{
    // Describe and create a constant buffer view.
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = CalcConstantBufferByteSize(sizeof(CommonCB));

    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(0, m_cbvsrvuavDescriptorSize);

    device->CreateConstantBufferView(&cbvDesc, hDescriptor);

}

void Scene::BuildTextureBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    // Create the texture.
    for(auto& fileName: m_DDSFileName)
    {
        ComPtr<ID3D12Resource> defaultBuffer;
        ComPtr<ID3D12Resource> uploadBuffer;

        // DDSTexture 를 사용하는 방식
        unique_ptr<uint8_t[]> ddsData;
        vector<D3D12_SUBRESOURCE_DATA> subresources;
        ThrowIfFailed(LoadDDSTextureFromFile(device, fileName.c_str(), defaultBuffer.GetAddressOf(), ddsData, subresources));

        //// DirectTex를 사용하는 방식
        //ScratchImage image;
        //TexMetadata metadata;

        //ThrowIfFailed(LoadFromDDSFile(L"./Textures/grass.dds", DDS_FLAGS_NONE, &metadata, image));
        ////metadata = image.GetMetadata(); // 이코드를 사용하고 위 코드의 3번째 인자를 nullptr로 해도 된다.

        //ThrowIfFailed(CreateTexture(device, metadata, m_textureBuffer_default.GetAddressOf()));
        //ThrowIfFailed(PrepareUpload(device, image.GetImages(), image.GetImageCount(), metadata, subresources));

        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(defaultBuffer.Get(), 0, subresources.size());
        
        OutputDebugStringA(string{ "current texture subresource size = " + to_string(subresources.size()) + "\n"}.c_str());
        OutputDebugStringA(string{ "current texture mip level = " + to_string(defaultBuffer->GetDesc().MipLevels) + "\n"}.c_str());
        OutputDebugStringA(string{ "current texture format = " + to_string(defaultBuffer->GetDesc().Format) + "\n"}.c_str());

        // Create the GPU upload buffer.
        ThrowIfFailed(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(uploadBuffer.GetAddressOf())));
        
        UpdateSubresources(commandList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, static_cast<UINT>(subresources.size()), subresources.data());
        commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

        m_textureBuffer_defaults.push_back(move(defaultBuffer));
        m_textureBuffer_uploads.push_back(move(uploadBuffer));
    }
}

void Scene::BuildTextureBufferView(ID3D12Device* device)
{
    // Describe and create a SRV for the texture.
    for (int i = 0; i < m_DDSFileName.size(); ++i)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = m_textureBuffer_defaults[i]->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = m_textureBuffer_defaults[i]->GetDesc().MipLevels;

        CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
        hDescriptor.Offset(1 + i, m_cbvsrvuavDescriptorSize); // 1 + i  에서 1의 의미는 이전에 만들어진 constant buffer view의 수 이다. 아직 한 개만 있음 

        device->CreateShaderResourceView(m_textureBuffer_defaults[i].Get(), &srvDesc, hDescriptor);
    }
}

UINT Scene::CalcConstantBufferByteSize(UINT byteSize)
{
    return (byteSize + 255) & ~255;
}

void Scene::BuildProjMatrix()
{
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI * 0.25f, m_viewport.Width / m_viewport.Height, 1.0f, 1000.0f);
    XMStoreFloat4x4(&m_proj, proj);
}

void Scene::SetState(ID3D12GraphicsCommandList* commandList)
{
    // Set necessary state.
    commandList->SetPipelineState(m_pipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    commandList->RSSetViewports(1, &m_viewport);
    commandList->RSSetScissorRects(1, &m_scissorRect);
}

void Scene::SetDescriptorHeaps(ID3D12GraphicsCommandList* commandList)
{
    ID3D12DescriptorHeap* ppHeaps[] = { m_descriptorHeap.Get()};
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}

// Update frame-based values.
void Scene::OnUpdate(GameTimer& gTimer)
{
    for (auto& [key, value] : m_objects)
    {
        visit([&gTimer](auto& arg) {arg.OnUpdate(gTimer); }, value);
    }

    //투영행렬 쉐이더로 전달
    memcpy(m_mappedData + 64, &XMMatrixTranspose(XMLoadFloat4x4(&m_proj)), sizeof(XMMATRIX)); // 처음 매개변수는 시작주소
}

// Render the scene.
void Scene::OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    //commandList->IASetIndexBuffer(&m_indexBufferView);
    //
    CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());
    commandList->SetGraphicsRootDescriptorTable(0, hDescriptor);

    for (auto& [key, value] : m_objects)
    {
        visit([&device, &commandList](auto& arg) {arg.OnRender(device, commandList); }, value);
    }
}

void Scene::OnResize(UINT width, UINT height)
{
    m_viewport = { CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f) };
    m_scissorRect = { CD3DX12_RECT(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)) };
    BuildProjMatrix();
}

void Scene::OnDestroy()
{
    CD3DX12_RANGE Range(0, CalcConstantBufferByteSize(sizeof(ObjectCB)));
    m_constantBuffer->Unmap(0, &Range);
}

void Scene::OnKeyDown(UINT8 key)
{

}

void Scene::OnKeyUp(UINT8 key)
{
}

std::wstring Scene::GetSceneName() const
{
    return m_name;
}

ResourceManager& Scene::GetResourceManager()
{
    return *(m_resourceManager.get());
}

UINT8* Scene::GetConstantBufferMappedData()
{
    // TODO: 여기에 return 문을 삽입합니다.
    return m_mappedData;
}

ID3D12DescriptorHeap* Scene::GetDescriptorHeap()
{
    return m_descriptorHeap.Get();
}
