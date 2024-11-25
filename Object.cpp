#include "Object.h"
#include "GameTimer.h"
#include "Scene.h"
#include "DXSampleHelper.h"

Object::Object(Scene* root) : m_root{ root }, m_mappedData{nullptr}
{
}

void Object::BuildConstantBuffer(ID3D12Device* device)
{
    const UINT constantBufferSize = m_root->CalcConstantBufferByteSize(sizeof(ObjectCB));    // CB size is required to be 256-byte aligned.

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
    ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(& m_mappedData)));

}

PlayerObject::PlayerObject(Scene* root) : Object{ root }
{
    mRotation = XMMatrixIdentity();
    
}

void PlayerObject::OnUpdate(GameTimer& gTimer)
{
    OnKeyboardInput(gTimer);

    //GetComponent<Scale>().SetXMVECTOR(GetComponent<Scale>().GetXMVECTOR() * GetComponent<Scale>().mScaleValue);
    XMMATRIX scale = XMMatrixScalingFromVector(GetComponent<Scale>().GetXMVECTOR());

    GetComponent<Rotation>().SetXMVECTOR(GetComponent<Rotation>().GetXMVECTOR() + GetComponent<Rotate>().GetXMVECTOR() * gTimer.DeltaTime());
    XMMATRIX rotate = XMMatrixRotationRollPitchYawFromVector(GetComponent<Rotation>().GetXMVECTOR() * (XM_PI / 180.0f));

    bool isEqual = XMVector4Equal(GetComponent<Velocity>().GetXMVECTOR(), XMVECTOR{ 0,0,0,0 });
    string fileName = isEqual ? "1P(boy-idle).fbx" : "1P(boy-walk).fbx";
    
    if (FindComponent<Gravity>()) {
        float& t = GetComponent<Gravity>().mGravityTime;
        t += gTimer.DeltaTime(); // t를 초기화 하는 조건도 생각해야함.
        float y = XMVectorGetY(GetComponent<Position>().GetXMVECTOR());
        if (y > 0) {
            fileName = "1P(boy-jump).fbx";
            GetComponent<Velocity>().SetXMVECTOR(XMVectorSetY(GetComponent<Velocity>().GetXMVECTOR(), 0.5*-9.8*(t*t)));
        }
        else {
            t = 0;
        }
    }

    GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    GetComponent<Velocity>().SetXMVECTOR(XMVECTOR{0,0,0,0});
    XMMATRIX translate = XMMatrixTranslationFromVector(GetComponent<Position>().GetXMVECTOR());

    XMMATRIX world = XMMatrixIdentity();

    // 월드 행렬 = 크기 행렬 * 회전 행렬 * 이동 행렬
    if (fileName == "1P(boy-jump).fbx")
        world = scale * mRotation * translate;
    else
        world = scale * mRotation * rotate * translate;

    memcpy(m_mappedData , &XMMatrixTranspose(world), sizeof(XMMATRIX)); // 처음 매개변수는 시작주소

    int isAnimate = FindComponent<Animation>();
    if (isAnimate) {
        vector<XMFLOAT4X4> finalTransforms{90};
        Animation& animComponent = GetComponent<Animation>();
        SkinnedData& animData = animComponent.mAnimData->at(fileName);
        animComponent.time += gTimer.DeltaTime();
        string clipName = "Take 001";
        if (fileName == "1P(boy-jump).fbx") clipName = "mixamo.com";
        if (animComponent.time >= animData.GetClipEndTime(clipName)) animComponent.time = 0.f;
        animData.GetFinalTransforms(clipName, animComponent.time, finalTransforms);
        memcpy(m_mappedData + sizeof(XMMATRIX), finalTransforms.data(), sizeof(XMMATRIX) * 90); // 처음 매개변수는 시작주소
    }
    memcpy(m_mappedData + sizeof(XMMATRIX) * 91, &isAnimate, sizeof(int));
};

void PlayerObject::OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(m_root->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(1+GetComponent<Texture>().mDescriptorStartIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    commandList->SetGraphicsRootDescriptorTable(1, hDescriptor);
    commandList->SetGraphicsRootConstantBufferView(2, m_constantBuffer.Get()->GetGPUVirtualAddress());
    //commandList->DrawIndexedInstanced(tmp.indexCountPerInstance, 1, tmp.statIndexLocation, tmp.baseVertexLocation, 0);
    commandList->DrawInstanced(GetComponent<Mesh>().mSubMeshData.vertexCountPerInstance, 1, GetComponent<Mesh>().mSubMeshData.startVertexLocation, 0);
}

void PlayerObject::OnKeyboardInput(const GameTimer& gTimer)
{
    float speed = 30;
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
        speed = 100;
    }

    XMMATRIX view = m_root->GetObj<CameraObject>(L"CameraObject").GetXMMATRIX();
    XMMATRIX invView = XMMatrixInverse(NULL,view);
    XMMATRIX transposeView = XMMatrixTranspose(view);

    XMVECTOR forward = XMVECTOR{ 0.f, 0.f, speed, 0.f };
    XMVECTOR forwardInv = XMVector4Transform(forward, invView);
    XMVECTOR right = XMVECTOR{ speed, 0.f, 0.f, 0.f };
    XMVECTOR rightInv = XMVector4Transform(right, invView);
    XMVECTOR forwardRight = XMVECTOR{ speed, 0.f, speed, 0.f };
    XMVECTOR forwardRightInv = XMVector4Transform(forwardRight, invView);
    XMVECTOR forwardLeft = XMVECTOR{ -speed, 0.f, speed, 0.f };
    XMVECTOR forwardLeftInv = XMVector4Transform(forwardLeft, invView);

    forward = XMVectorSetY(forward, 0.f);
    forwardInv = XMVectorSetY(forwardInv, 0.f);
    forwardRight = XMVectorSetY(forwardRight, 0.f);
    forwardRightInv = XMVectorSetY(forwardRightInv, 0.f);
    forwardLeft = XMVectorSetY(forwardLeft, 0.f);
    forwardLeftInv = XMVectorSetY(forwardLeftInv, 0.f);

    XMVECTOR up = XMVECTOR{ 0.f, 1.f, 0.f, 0.f };


    bool isW = GetAsyncKeyState('W') & 0x8000;
    bool isS = GetAsyncKeyState('S') & 0x8000;
    bool isA = GetAsyncKeyState('A') & 0x8000;
    bool isD = GetAsyncKeyState('D') & 0x8000;

    bool isJump = GetAsyncKeyState(VK_SPACE) & 0x8000;

    if (isW) {
        GetComponent<Velocity>().SetXMVECTOR(forwardInv);
        mRotation = XMMATRIX(XMVector4Normalize(rightInv) , XMVector4Normalize(up), XMVector4Normalize(forwardInv), XMVECTOR{0.0f, 0.0f, 0.0f, 1.0f});
    }
    if (isS) {
        GetComponent<Velocity>().SetXMVECTOR(-forwardInv);
        mRotation = XMMATRIX(XMVector4Normalize(-rightInv), up, XMVector4Normalize(-forwardInv), XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });
    }
    if (isA) {
        GetComponent<Velocity>().SetXMVECTOR(-rightInv);
        mRotation = XMMATRIX(XMVector4Normalize(forwardInv), up, XMVector4Normalize(-rightInv), XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });
    }
    if (isD) {
        GetComponent<Velocity>().SetXMVECTOR(rightInv);
        mRotation = XMMATRIX(XMVector4Normalize(-forwardInv), up, XMVector4Normalize(rightInv), XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });
    }
    if (isJump) {
        GetComponent<Velocity>().SetXMVECTOR(XMVectorSetY(GetComponent<Velocity>().GetXMVECTOR(), 200));
    }
    //if (isW && isD) {
    //    GetComponent<Velocity>().SetXMVECTOR(forwardRightInv);
    //    mRotation = XMMATRIX(XMVector4Normalize(-forwardLeft), up, XMVector4Normalize(forwardRight), XMVECTOR{0.0f, 0.0f, 0.0f, 1.0f});
    //}
    //if (isW && isA) {
    //    GetComponent<Velocity>().SetXMVECTOR(forwardLeftInv);
    //    mRotation = XMMATRIX(XMVector4Normalize(forwardRight), up, XMVector4Normalize(forwardLeft), XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });
    //}
    //if (isS && isD){ 
    //    GetComponent<Velocity>().SetXMVECTOR(-forwardLeftInv);
    //    mRotation = XMMATRIX(XMVector4Normalize(-forwardRightInv), up, XMVector4Normalize(-forwardLeftInv), XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });
    //}
    //if (isS && isA) {
    //    GetComponent<Velocity>().SetXMVECTOR(-forwardRightInv);
    //    mRotation = XMMATRIX(XMVector4Normalize(forwardLeft), up, XMVector4Normalize(-forwardRight), XMVECTOR{ 0.0f, 0.0f, 0.0f, 1.0f });
    //}

    if (GetAsyncKeyState('P') & 0x8000) {
        XMFLOAT4 pos = GetComponent<Position>().mFloat4;
        OutputDebugStringA(string{ to_string(pos.x) + "," + to_string(pos.y) + "," + to_string(pos.z) + "\n"}.c_str());
    }
}

TestObject::TestObject(Scene* root) : Object{root}
{
}

void TestObject::OnUpdate(GameTimer& gTimer)
{
    //GetComponent<Scale>().SetXMVECTOR(GetComponent<Scale>().GetXMVECTOR() * GetComponent<Scale>().mScaleValue);
    XMMATRIX scale = XMMatrixScalingFromVector(GetComponent<Scale>().GetXMVECTOR());

    GetComponent<Rotation>().SetXMVECTOR(GetComponent<Rotation>().GetXMVECTOR() + GetComponent<Rotate>().GetXMVECTOR() * gTimer.DeltaTime());
    XMMATRIX rotate = XMMatrixRotationRollPitchYawFromVector(GetComponent<Rotation>().GetXMVECTOR() * (XM_PI / 180.0f));
    
    if (FindComponent<Gravity>()) {
        float& t = GetComponent<Gravity>().mGravityTime;
        t += gTimer.DeltaTime(); // t를 초기화 하는 조건도 생각해야함.
        float y = XMVectorGetY(GetComponent<Position>().GetXMVECTOR());
        if (y > 0) {
            GetComponent<Velocity>().SetXMVECTOR(XMVectorSetY(GetComponent<Velocity>().GetXMVECTOR(), 0.5 * -9.8 * (t * t)));
        }
        else {
            t = 0;
        }
    }

    GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    GetComponent<Velocity>().SetXMVECTOR(XMVECTOR{ 0,0,0,0 });
    XMMATRIX translate = XMMatrixTranslationFromVector(GetComponent<Position>().GetXMVECTOR());

    // 월드 행렬 = 크기 행렬 * 회전 행렬 * 이동 행렬
    XMMATRIX world = scale * rotate * translate;
    
    //애니메이션 유무
    memcpy(m_mappedData, &XMMatrixTranspose(world), sizeof(XMMATRIX)); // 처음 매개변수는 시작주소

    int isAnimate = FindComponent<Animation>();
    if (isAnimate) {
        vector<XMFLOAT4X4> finalTransforms{ 90 };
        Animation& animComponent = GetComponent<Animation>();
        SkinnedData& animData = animComponent.mAnimData->at("humanoid.fbx");
        animComponent.time += gTimer.DeltaTime();
        if (animComponent.time > animData.GetClipEndTime("punch")) animComponent.time = 0.f;
        animData.GetFinalTransforms("punch", animComponent.time, finalTransforms);
        memcpy(m_mappedData + sizeof(XMFLOAT4X4), finalTransforms.data(), sizeof(XMFLOAT4X4) * 90); // 처음 매개변수는 시작주소
    }
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91, &isAnimate, sizeof(int));
}

void TestObject::OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(m_root->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(1 + GetComponent<Texture>().mDescriptorStartIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    commandList->SetGraphicsRootDescriptorTable(1, hDescriptor);
    commandList->SetGraphicsRootConstantBufferView(2, m_constantBuffer.Get()->GetGPUVirtualAddress());
    //commandList->DrawIndexedInstanced(tmp.indexCountPerInstance, 1, tmp.statIndexLocation, tmp.baseVertexLocation, 0);
    commandList->DrawInstanced(GetComponent<Mesh>().mSubMeshData.vertexCountPerInstance, 1, GetComponent<Mesh>().mSubMeshData.startVertexLocation, 0);
}

CameraObject::CameraObject(float radius, Scene* root) :
    Object{ root }, 
    mLastPosX{ -1 }, 
    mLastPosY{ -1 }, 
    mTheta{XMConvertToRadians(-90)}, 
    mPhi{ XMConvertToRadians(70) }, 
    mRadius{radius}
{
}

void CameraObject::OnUpdate(GameTimer& gTimer)
{
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float y = mRadius * cosf(mPhi);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);

    XMVECTOR playerPos = m_root->GetObj<PlayerObject>(L"PlayerObject").GetComponent<Position>().GetXMVECTOR();
    GetComponent<Position>().SetXMVECTOR(playerPos + XMVECTOR{ x, y, z, 0.f });

    // 카메라 변환 행렬.
    XMVECTOR eye = GetComponent<Position>().GetXMVECTOR();
    XMVECTOR target = playerPos;
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    SetXMMATRIX(XMMatrixLookAtLH(eye, target, up));
    // 카메라 변환 행렬 쉐이더에 전달
    memcpy(m_root->GetConstantBufferMappedData(), &XMMatrixTranspose(GetXMMATRIX()), sizeof(XMMATRIX)); // 처음 매개변수는 시작주소
}

void CameraObject::OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
}

void CameraObject::OnMouseInput(WPARAM wParam, int width, int height)
{
    POINT currentMousePos;
    GetCursorPos(&currentMousePos);
    float dx = static_cast<float>(currentMousePos.x - width/2.f);
    float dy = static_cast<float>(currentMousePos.y - height/2.f);
    mTheta -= XMConvertToRadians(dx * 0.01f);
    mPhi -= XMConvertToRadians(dy * 0.01f);


    SetCursorPos(width / 2.f, height / 2.f);


    // 각도 clamp
    float min = 0.1f;
    float max = XM_PI - 0.1f;
    mPhi = mPhi < min ? min : (mPhi > max ? max : mPhi);

}

void CameraObject::SetXMMATRIX(XMMATRIX m)
{
    XMStoreFloat4x4(&mViewMatrix, m);
}

XMMATRIX& CameraObject::GetXMMATRIX()
{
    return XMLoadFloat4x4(&mViewMatrix);
}
