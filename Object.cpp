#include "Object.h"
#include "GameTimer.h"
#include "Scene.h"
#include "DXSampleHelper.h"
#include <random>
#include "Framework.h"

std::random_device rdX;  // 첫 번째 rd 객체
std::random_device rdZ;  // 두 번째 rd 객체
default_random_engine dreX(rdX());
default_random_engine dreZ(rdZ());
uniform_int_distribution uidX(-1,1);
uniform_int_distribution uidZ(-1,1);

Object::~Object()
{
    for (Component* component : m_components) {
        delete component;
    }
}

Object::Object(Scene* root) : m_parent{ root }, m_mappedData{nullptr}
{
}

void Object::OnUpdate(GameTimer& gTimer)
{

}
void Object::LateUpdate(GameTimer& gTimer)
{

}

void Object::OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    Mesh* mesh = GetComponent<Mesh>();
    if (!mesh) return;

    CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(m_parent->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(1 + GetComponent<Texture>()->mDescriptorStartIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    commandList->SetGraphicsRootDescriptorTable(1, hDescriptor);
    commandList->SetGraphicsRootConstantBufferView(2, m_constantBuffer.Get()->GetGPUVirtualAddress());
    SubMeshData& data = GetComponent<Mesh>()->mSubMeshData;
    if (data.startIndexLocation == -1) {
        commandList->DrawInstanced(data.vertexCountPerInstance, 1, data.startVertexLocation, 0);
    }
    else {
        commandList->DrawIndexedInstanced(data.indexCountPerInstance, 1, data.startIndexLocation, data.baseVertexLocation, 0);
    }

}


void Object::BuildConstantBuffer(ID3D12Device* device)
{
    const UINT constantBufferSize = m_parent->CalcConstantBufferByteSize(sizeof(ObjectCB));    // CB size is required to be 256-byte aligned.

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

void Object::AddComponent(Component* component)
{
    m_components.push_back(component);
}

PlayerObject::PlayerObject(Scene* root) : Object{ root } , mRotation{ XMMatrixIdentity()}
{
}

void PlayerObject::OnUpdate(GameTimer& gTimer)
{
    OnKeyboardInput(gTimer);
   
    string currentFileName = "boy_walk_fix.fbx";
    //switch (GetComponent<StateMachine>().mCurrentState)
    //{
    //case eBehavior::Idle:
    //    currentFileName = "1P(boy-idle).fbx";
    //    break;
    //case eBehavior::Walk:
    //    currentFileName = "boy_walk_fix.fbx";
    //    break;
    //case eBehavior::Run:
    //    currentFileName = "boy_run_fix.fbx";
    //    break;
    //default:
    //    break;
    //}

    Animation* animation = GetComponent<Animation>();
    int isAnimate = false;
    if (animation) {
        isAnimate = true;
        vector<XMFLOAT4X4> finalTransforms{90};
        SkinnedData& animData = animation->mAnimData->at(currentFileName);
        animation->mAnimationTime += gTimer.DeltaTime();
        string clipName = "Take 001";
        if (animation->mAnimationTime >= animData.GetClipEndTime(clipName)) animation->mAnimationTime = 0.0f;
        animData.GetFinalTransforms(clipName, animation->mAnimationTime, finalTransforms);
        memcpy(m_mappedData + sizeof(XMMATRIX), finalTransforms.data(), sizeof(XMMATRIX) * 90); // 처음 매개변수는 시작주소
    }
    memcpy(m_mappedData + sizeof(XMMATRIX) * 91, &isAnimate, sizeof(int));
    float powValue = 1.f; // 짝수이면 안됨
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4, &powValue, sizeof(float));
    float ambiantValue = 0.4f;
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4 + sizeof(float), &ambiantValue, sizeof(float));
}

void PlayerObject::LateUpdate(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    XMVECTOR pos = transform->GetPosition();
    char outstatus = m_parent->ClampToBounds(pos, {0.0f, 0.0f, 0.0f});
    transform->SetPosition(pos);

    XMMATRIX world = transform->GetTransformM();
    XMMATRIX adjustScaleM = XMMatrixScaling(0.1f, 0.1f, 0.1f);
    XMMATRIX adjustTranslateM = XMMatrixIdentity();
    XMMATRIX adjustRotationM = XMMatrixIdentity();
    XMMATRIX adjustM = adjustScaleM * adjustRotationM * adjustTranslateM;

    memcpy(m_mappedData, &XMMatrixTranspose(adjustM * world), sizeof(XMMATRIX));  
}

void PlayerObject::OnKeyboardInput(const GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    BYTE* keyState = m_parent->GetFramework()->GetKeyState();

    XMVECTOR dir = XMVectorZero();
    if ((keyState[0x57] & 0x88) == 0x88) { dir += XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); } // w
    if ((keyState[0x53] & 0x88) == 0x88) { dir -= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); } // s
    if ((keyState[0x41] & 0x88) == 0x88) { dir -= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); } // a
    if ((keyState[0x44] & 0x88) == 0x88) { dir += XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); } // d

    float speed = 15;
    if ((keyState[VK_SHIFT] & 0x88) == 0x88) {
        speed = 30;
    }

    if ((keyState[VK_CONTROL] & 0x88) == 0x88) {
        speed = 100;
    }

    if (!XMVector3Equal(dir, XMVectorZero())) {
        CameraObject* cameraObj = m_parent->GetObj<CameraObject>();
        Transform* cameraTransform = cameraObj->GetComponent<Transform>();
        dir = XMVector3TransformNormal(dir, cameraTransform->GetRotationM());

        dir = XMVector3Normalize(XMVectorSetY(dir, 0.0f));

        XMVECTOR pos = transform->GetPosition();
        pos += dir * speed * gTimer.DeltaTime();
        transform->SetPosition(pos);

        float yaw = atan2f(XMVectorGetX(dir), XMVectorGetZ(dir)) * 180 / 3.141592f;
        transform->SetRotation({ 0.0f, yaw, 0.0f });
    }
}

TestObject::TestObject(Scene* root) : Object{root}
{
}

void TestObject::OnUpdate(GameTimer& gTimer)
{

    int isAnimate = false;
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91, &isAnimate, sizeof(int));
    float powValue = 1.f; // 짝수이면 안됨
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4, &powValue, sizeof(float));
    float ambiantValue = 0.2f;
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4 + sizeof(float), &ambiantValue, sizeof(float));
}

void TestObject::LateUpdate(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    XMMATRIX world = transform->GetTransformM();
    memcpy(m_mappedData, &XMMatrixTranspose(world), sizeof(XMMATRIX));
}

CameraObject::CameraObject(Scene* root, float radius) :
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

    Object* playerObj = m_parent->GetObj<PlayerObject>();
    Transform* playerTransform = playerObj->GetComponent<Transform>();
    XMVECTOR playerPos = playerTransform->GetPosition();

    Transform* myTransform = GetComponent<Transform>();
    myTransform->SetPosition(playerPos + XMVECTOR{ x, y, z, 0.f });
    
    XMVECTOR myPos = myTransform->GetPosition();
    XMVECTOR dir = playerPos - myPos;

    XMFLOAT3 yawPitch{};
    XMStoreFloat3(&yawPitch, dir);

    float yaw = atan2f(yawPitch.x, yawPitch.z) * 180 / 3.141592f;
    float pitch = atan2f(yawPitch.y, sqrtf(yawPitch.x * yawPitch.x + yawPitch.z * yawPitch.z)) * 180 / 3.141592f;
    myTransform->SetRotation({ -pitch, yaw, 0.0f });
}

void CameraObject::LateUpdate(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    XMVECTOR pos = transform->GetPosition();
    char outstatus = m_parent->ClampToBounds(pos, {0.0f, 1.0f, 0.0f});
    transform->SetPosition(pos);

    XMMATRIX transformM = transform->GetTransformM();
    XMMATRIX invtransformM = XMMatrixInverse(nullptr, transformM);
    memcpy(m_parent->GetConstantBufferMappedData(), &XMMatrixTranspose(invtransformM), sizeof(XMMATRIX)); // 처음 매개변수는 시작주소

}

void CameraObject::OnMouseInput(WPARAM wParam, HWND hWnd)
{
    // 현재 wnd의 센터 좌표를 알아온다
    RECT clientRect{};
    GetWindowRect(hWnd, &clientRect);
    int width = int(clientRect.right - clientRect.left);
    int height = int(clientRect.bottom - clientRect.top);
    int centerX = clientRect.left + width / 2;
    int centerY = clientRect.top + height / 2;

    POINT currentMousePos;
    GetCursorPos(&currentMousePos);
    float dx = static_cast<float>(currentMousePos.x - centerX);
    float dy = static_cast<float>(currentMousePos.y - centerY);
    mTheta -= XMConvertToRadians(dx * 0.02f);
    mPhi -= XMConvertToRadians(dy * 0.02f);

    // 각도 clamp
    float min = 0.1f;
    float max = XM_PI - 0.1f;
    mPhi = mPhi < min ? min : (mPhi > max ? max : mPhi);

    SetCursorPos(centerX, centerY);
}

TerrainObject::TerrainObject(Scene* root) : Object{ root }
{
}

void TerrainObject::OnUpdate(GameTimer& gTimer)
{
    int isAnimate = false;
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91, &isAnimate, sizeof(int));
    float powValue = 5.f; // 짝수이면 안됨
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4, &powValue, sizeof(float));
    float ambiantValue = 0.4f;
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4 + sizeof(float), &ambiantValue, sizeof(float));
}

void TerrainObject::LateUpdate(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    XMMATRIX world = transform->GetTransformM();
    memcpy(m_mappedData, &XMMatrixTranspose(world), sizeof(XMMATRIX));
}

TreeObject::TreeObject(Scene* root) : Object{ root }
{
}

void TreeObject::OnUpdate(GameTimer& gTimer)
{
    int isAnimate = false;
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91, &isAnimate, sizeof(int));
    float powValue = 1.f; // 짝수이면 안됨
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4, &powValue, sizeof(float));
    float ambiantValue = 0.4f;
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4 + sizeof(float), &ambiantValue, sizeof(float));
}

void TreeObject::LateUpdate(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    XMVECTOR pos = transform->GetPosition();
    char outstatus = m_parent->ClampToBounds(pos, { 0.0f, 0.0f, 0.0f });
    transform->SetPosition(pos);

    XMMATRIX world = transform->GetTransformM();

    XMMATRIX adjustScaleM = XMMatrixScaling(20.0f, 20.0f, 20.0f);
    XMMATRIX adjustTranslateM = XMMatrixTranslation(-16.5f, 4.5f, -50.f);
    XMMATRIX adjustRotationM = XMMatrixIdentity();
    XMMATRIX adjustM = adjustScaleM * adjustRotationM * adjustTranslateM;

    memcpy(m_mappedData, &XMMatrixTranspose(adjustM * world), sizeof(XMMATRIX));
}

TigerObject::TigerObject(Scene* root) : Object{ root }, mTimer{10.0f}
{
}

void TigerObject::OnUpdate(GameTimer& gTimer)
{
    // RandomVelocity(gTimer);
    TigerBehavior(gTimer);
    Transform* transform = GetComponent<Transform>();

    std::string currentFileName = "202411_walk_tiger_center.fbx";

    Animation* animation = GetComponent<Animation>();
    int isAnimate = false;
    if (animation) {
        isAnimate = true;
        vector<XMFLOAT4X4> finalTransforms{ 90 };
        SkinnedData& animData = animation->mAnimData->at(currentFileName);
        animation->mAnimationTime += gTimer.DeltaTime();
        string clipName = "Take 001";
        if (animation->mAnimationTime >= animData.GetClipEndTime(clipName)) animation->mAnimationTime = 0.f;
        animData.GetFinalTransforms(clipName, animation->mAnimationTime, finalTransforms);
        memcpy(m_mappedData + sizeof(XMMATRIX), finalTransforms.data(), sizeof(XMMATRIX) * 90); // 처음 매개변수는 시작주소
    }
    memcpy(m_mappedData + sizeof(XMMATRIX) * 91, &isAnimate, sizeof(int));
    float powValue = 1.f; // 짝수이면 안됨
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4, &powValue, sizeof(float));
    float ambiantValue = 0.4f;
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4 + sizeof(float), &ambiantValue, sizeof(float));
}

void TigerObject::LateUpdate(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    XMVECTOR pos = transform->GetPosition();
    char outstatus = m_parent->ClampToBounds(pos, { 0.0f, 0.0f, 0.0f });
    transform->SetPosition(pos);
    
    XMMATRIX world = transform->GetTransformM();

    XMMATRIX adjustScaleM = XMMatrixScaling(0.2f, 0.2f, 0.2f);
    XMMATRIX adjustRotM = XMMatrixRotationRollPitchYaw(0.0f, XMConvertToRadians(180.0f), 0.0f);
    XMMATRIX adjustTranslateM = XMMatrixTranslation(0.0f, 0.0f, -8.0f);
    XMMATRIX adjustM = adjustScaleM * adjustRotM * adjustTranslateM;

    memcpy(m_mappedData, &XMMatrixTranspose(adjustM * world), sizeof(XMMATRIX)); // 처음 매개변수는 시작주소
}

void TigerObject::TigerBehavior(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    XMVECTOR pos = transform->GetPosition();

    PlayerObject* player = m_parent->GetObj<PlayerObject>();
    Transform* playerTransform = player->GetComponent<Transform>();
    XMVECTOR playerPos = playerTransform->GetPosition();

    XMVECTOR vec = playerPos - pos;
    
    float speed = 15.f;
    float result = XMVectorGetX(XMVector3Length(vec));
    if (result < 200.f) {
        vec = XMVector3Normalize(XMVectorSetY(vec, 0.0f));
        transform->SetPosition(pos + vec * speed * gTimer.DeltaTime());
               
        float yaw = atan2f(XMVectorGetX(vec), XMVectorGetZ(vec)) * 180 / 3.141592f;
        transform->SetRotation({ 0.0f, yaw, 0.0f });
    }
    else {
        // 탐색 시 행동
    }
}

//void TigerObject::RandomVelocity(GameTimer& gTimer)
//{
//
//    mTimer += gTimer.DeltaTime();
//    XMVECTOR pos = GetComponent<Position>().GetXMVECTOR();
//    if (XMVectorGetX(pos) <= 0.f) {
//        mTimer = 0.f;
//        //float value = static_cast<float>(uid(dre)) ? 1.f : -1.f;
//        mTempVelocity = {1.f, 0.f, (float)uidZ(dreZ)};
//    }
//
//    if (XMVectorGetZ(pos) <= 0.f) {
//        mTimer = 0.f;
//        //float value = static_cast<float>(uid(dre)) ? 1.f : -1.f;
//        mTempVelocity = { (float)uidX(dreX), 0.f, 1.f };
//
//    }
//
//    if (mTimer >= 5.f) {
//        mTimer = 0.f;
//        //float value = static_cast<float>(uid(dre)) ? 0.5f : -1.f;
//        float x = (float)uidX(dreX);
//        float z = (float)uidZ(dreZ);
//        if (x == 0 && z == 0) {
//            mTempVelocity = { x, 0.f, 1.f };
//        }
//        mTempVelocity = { x, 0.f, z };
//    }
//}

StoneObject::StoneObject(Scene* root) : Object{root}
{
}

void StoneObject::OnUpdate(GameTimer& gTimer)
{
}

void StoneObject::LateUpdate(GameTimer& gTimer)
{
}