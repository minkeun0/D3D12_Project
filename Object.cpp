#include "Object.h"
#include "GameTimer.h"
#include "Scene.h"

Object::Object(Scene* root) : m_root{root}
{
}

PlayerObject::PlayerObject(Scene* root) : Object{ root }
{
}

void PlayerObject::OnUpdate(GameTimer& gTimer)
{
    //GetComponent<Scale>().SetXMVECTOR(GetComponent<Scale>().GetXMVECTOR() * GetComponent<Scale>().mScaleValue);
    XMMATRIX scale = XMMatrixScalingFromVector(GetComponent<Scale>().GetXMVECTOR());

    GetComponent<Rotation>().SetXMVECTOR(GetComponent<Rotation>().GetXMVECTOR() + GetComponent<Rotate>().GetXMVECTOR() * gTimer.DeltaTime());
    XMMATRIX rotate = XMMatrixRotationRollPitchYawFromVector(GetComponent<Rotation>().GetXMVECTOR() * (XM_PI / 180.0f));

    OnKeyboardInput(gTimer);
    XMMATRIX translate = XMMatrixTranslationFromVector(GetComponent<Position>().GetXMVECTOR());

    // 월드 행렬 = 크기 행렬 * 회전 행렬 * 이동 행렬
    XMMATRIX world = scale * rotate * translate;
    GetComponent<World>().SetXMMATRIX(world);

};

void PlayerObject::OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(m_root->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(1+GetComponent<Texture>().mDescriptorStartIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    commandList->SetGraphicsRootDescriptorTable(1, hDescriptor);
    commandList->SetGraphicsRoot32BitConstants(2, 16, &XMMatrixTranspose(GetComponent<World>().GetXMMATRIX()), 0);
    int isAnimate = FindComponent<Animation>();
    commandList->SetGraphicsRoot32BitConstants(3, 1, &isAnimate, 0);
    //commandList->DrawIndexedInstanced(tmp.indexCountPerInstance, 1, tmp.statIndexLocation, tmp.baseVertexLocation, 0);
    commandList->DrawInstanced(GetComponent<Mesh>().mSubMeshData.vertexCountPerInstance, 1, GetComponent<Mesh>().mSubMeshData.startVertexLocation, 0);
}

void PlayerObject::OnKeyboardInput(const GameTimer& gTimer)
{
    float speed = 10;
    if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
        speed = 100;
    }

    XMMATRIX view = m_root->GetObj<CameraObject>(L"CameraObject").GetXMMATRIX();
    XMVECTOR forward = XMVector4Transform(XMVECTOR{ 0, 0, speed, 0 }, XMMatrixTranspose(view));
    XMVECTOR right = XMVector4Transform(XMVECTOR{ speed, 0, 0, 0 }, XMMatrixTranspose(view));

    //forward = XMVectorSetY(forward, 0.f);

    if (GetAsyncKeyState('W') & 0x8000) {
        GetComponent<Velocity>().SetXMVECTOR(forward);
        GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        GetComponent<Velocity>().SetXMVECTOR(- forward);
        GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        GetComponent<Velocity>().SetXMVECTOR(-right);
        GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        GetComponent<Velocity>().SetXMVECTOR(right);
        GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    }

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

    GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    XMMATRIX translate = XMMatrixTranslationFromVector(GetComponent<Position>().GetXMVECTOR());

    // 월드 행렬 = 크기 행렬 * 회전 행렬 * 이동 행렬
    GetComponent<World>().SetXMMATRIX(scale * rotate * translate);
    
    //애니메이션 유무
    int isAnimate = FindComponent<Animation>();
    if (isAnimate) {
        vector<XMFLOAT4X4> finalTransforms;
        Animation& animComponent = GetComponent<Animation>();
        SkinnedData* animData = animComponent.mAnimData;
        animComponent.time += gTimer.DeltaTime();
        if (animComponent.time > animData->GetClipEndTime("punch")) animComponent.time = 0.f;
        animData->GetFinalTransforms("punch", animComponent.time, finalTransforms);
        memcpy(m_root->GetConstantBufferMappedData() + sizeof(XMFLOAT4X4) * 2, finalTransforms.data(), sizeof(XMFLOAT4X4) * 90); // 처음 매개변수는 시작주소
    }
}

void TestObject::OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(m_root->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(1 + GetComponent<Texture>().mDescriptorStartIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    commandList->SetGraphicsRootDescriptorTable(1, hDescriptor);
    commandList->SetGraphicsRoot32BitConstants(2, 16, &XMMatrixTranspose(GetComponent<World>().GetXMMATRIX()), 0);
    int isAnimate = FindComponent<Animation>();
    commandList->SetGraphicsRoot32BitConstants(3, 1, &isAnimate, 0);
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

void CameraObject::OnMouseInput(WPARAM wParam, int x, int y)
{
    //OutputDebugStringA((to_string(x) + " : " + to_string(y) + "\n").c_str());
    //if (mLastPosX == -1 || mLastPosY == -1) {
    //    mLastPosX = x;
    //    mLastPosY = y;
    //}
    if ((wParam & MK_LBUTTON) != 0) {
        float dx = static_cast<float>(x - mLastPosX);
        float dy = static_cast<float>(y - mLastPosY);
        mTheta -= XMConvertToRadians(dx * 0.1f);
        mPhi -= XMConvertToRadians(dy * 0.1f);

        // 각도 clamp
        float min = 0.1f;
        float max = XM_PI - 0.1f;
        mPhi = mPhi < min ? min : (mPhi > max ? max : mPhi);
    };

    mLastPosX = x;
    mLastPosY = y;
}

void CameraObject::SetXMMATRIX(XMMATRIX m)
{
    XMStoreFloat4x4(&mViewMatrix, m);
}

XMMATRIX& CameraObject::GetXMMATRIX()
{
    return XMLoadFloat4x4(&mViewMatrix);
}
