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

Object::Object(Scene* scene, uint32_t id, uint32_t parentId) : m_scene{ scene }, m_id{id}, m_parent_id{parentId}
{
    BuildConstantBuffer(scene->GetFramework()->GetDevice());
}

void Object::OnUpdate(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    Gravity* gravity = GetComponent<Gravity>();
    if (gravity)
    {
        XMVECTOR newPos = gravity->ProcessGravity(transform->GetPosition(), gTimer.DeltaTime());
        transform->SetPosition(newPos);
    }

    XMMATRIX finalM = transform->GetTransformM();
    if (m_parent_id != -1) {
        Object* parentObj = m_scene->GetObjFromId(m_parent_id);
        if (parentObj) {
            Transform* parentTransform = parentObj->GetComponent<Transform>();
            finalM = finalM * parentTransform->GetFinalM();
        }
        else {
            // 부모가 있었지만 현재는 없는상태
            // 이런경우 일단 해당 오브젝트 삭제
        }
    }
    transform->SetFinalM(finalM);

    Collider* collider = GetComponent<Collider>();
    if (collider) {
        collider->UpdateOBB(finalM);
    }
}
void Object::OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration)
{
    float similarity = XMVectorGetX(XMVector3Dot(XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f }, -collisionNormal));
    Gravity* gravity = GetComponent<Gravity>();
    if (gravity && similarity > 0.95f) {
        gravity->ResetElapseTime();

        Transform* transform = GetComponent<Transform>();
        XMVECTOR pos = transform->GetPosition();
        pos += -collisionNormal * penetration;
        transform->SetPosition(pos);
    }
}

void Object::LateUpdate(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    TerrainObject* terrainObj = dynamic_cast<TerrainObject*>(this);
    if (!terrainObj && m_parent_id == -1) {
        XMVECTOR pos = transform->GetPosition();
        char outstatus = m_scene->ClampToBounds(pos, { 0.0f, 0.0f, 0.0f });
        transform->SetPosition(pos);

        Gravity* gravity = GetComponent<Gravity>();
        if ((outstatus & 0x04) && gravity)
        {
            gravity->ResetElapseTime();
        }
    }

    XMMATRIX world = transform->GetFinalM();
    XMMATRIX adjustM = XMMatrixIdentity();
    AdjustTransform* adjustTrnasform = GetComponent<AdjustTransform>();
    if (adjustTrnasform) {
        adjustM = adjustTrnasform->GetTransformM();
    }
    memcpy(m_mappedData, &XMMatrixTranspose(adjustM * world), sizeof(XMMATRIX));

    ProcessAnimation(gTimer);

    Texture* texture = GetComponent<Texture>();
    float powValue = 1.0f;
    float ambiantValue = 0.4f;
    if (texture) {
        ambiantValue = texture->mAmbiantValue;
        powValue = texture->mPowValue;
    }
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4, &powValue, sizeof(float));
    memcpy(m_mappedData + sizeof(XMFLOAT4X4) * 91 + sizeof(int) * 4 + sizeof(float), &ambiantValue, sizeof(float));
}

void Object::OnRender(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    Mesh* mesh = GetComponent<Mesh>();
    if (!mesh) return;
    
    Texture* texture = GetComponent<Texture>();
    int textureIndex = m_scene->GetTextureIndex(texture->mName);

    CD3DX12_GPU_DESCRIPTOR_HANDLE hDescriptor(m_scene->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
    hDescriptor.Offset(1 + textureIndex, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
    commandList->SetGraphicsRootDescriptorTable(1, hDescriptor);
    commandList->SetGraphicsRootConstantBufferView(2, m_constantBuffer.Get()->GetGPUVirtualAddress());

    SubMeshData& data = m_scene->GetResourceManager().GetSubMeshData(mesh->mName);
    if (data.startIndexLocation == -1) {
        commandList->DrawInstanced(data.vertexCountPerInstance, 1, data.startVertexLocation, 0);
    }
    else {
        commandList->DrawIndexedInstanced(data.indexCountPerInstance, 1, data.startIndexLocation, data.baseVertexLocation, 0);
    }

}


void Object::BuildConstantBuffer(ID3D12Device* device)
{
    const UINT constantBufferSize = m_scene->CalcConstantBufferByteSize(sizeof(ObjectCB));    // CB size is required to be 256-byte aligned.

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer)));

    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(& m_mappedData)));
}

void Object::AddComponent(Component* component)
{
    m_components.push_back(component);
}

void Object::ProcessAnimation(GameTimer& gTimer)
{
    Animation* animation = GetComponent<Animation>();
    int isAnimate = false;
    if (animation) {
        isAnimate = true;
        vector<XMFLOAT4X4> finalTransforms{ 90 };
        SkinnedData& animData = m_scene->GetResourceManager().GetAnimationData(animation->mCurrentFileName);
        animation->mAnimationTime += gTimer.DeltaTime();
        string clipName = "Take 001";
        if (animation->mAnimationTime >= animData.GetClipEndTime(clipName)) animation->mAnimationTime = 0.0f;
        animData.GetFinalTransforms(clipName, animation->mAnimationTime, finalTransforms);
        memcpy(m_mappedData + sizeof(XMMATRIX), finalTransforms.data(), sizeof(XMMATRIX) * 90); // 처음 매개변수는 시작주소
    }
    memcpy(m_mappedData + sizeof(XMMATRIX) * 91, &isAnimate, sizeof(int));
}

uint32_t Object::GetId()
{
    return m_id;
}

bool Object::GetValid()
{
    return m_valid;
}

void Object::Delete()
{
    m_valid = false;
}

void PlayerObject::OnUpdate(GameTimer& gTimer)
{
    OnKeyboardInput(gTimer);
    //string currentFileName = "boy_walk_fix.fbx";
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
    Object::OnUpdate(gTimer);
}

void PlayerObject::OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration)
{
    Transform* transform = GetComponent<Transform>();
    XMVECTOR pos = transform->GetPosition();
    pos += -collisionNormal * penetration;
    transform->SetPosition(pos);

    float similarity = XMVectorGetX(XMVector3Dot(XMVECTOR{ 0.0f, 1.0f, 0.0f, 0.0f }, -collisionNormal));
    Gravity* gravity = GetComponent<Gravity>();
    if (gravity && similarity > 0.95f) {
        gravity->ResetElapseTime();
    }
}

void PlayerObject::OnKeyboardInput(const GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    BYTE* keyState = m_scene->GetFramework()->GetKeyState();

    XMVECTOR dir = XMVectorZero();
    if ((keyState[0x57] & 0x88) == 0x88) { dir += XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); } // w
    if ((keyState[0x53] & 0x88) == 0x88) { dir -= XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); } // s
    if ((keyState[0x41] & 0x88) == 0x88) { dir -= XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); } // a
    if ((keyState[0x44] & 0x88) == 0x88) { dir += XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f); } // d

    float speed = 15;
    
    Gravity* gravity = GetComponent<Gravity>();
    if ((keyState[VK_SPACE] & 0x88) == 0x80 && gravity) {
        gravity->SetVerticalSpeed(20.0f);
    }
    
    if ((keyState[VK_SHIFT] & 0x88) == 0x88) {
        speed = 30;
    }

    if ((keyState[VK_CONTROL] & 0x88) == 0x88) {
        speed = 100;
    }

    if (!XMVector3Equal(dir, XMVectorZero())) {
        CameraObject* cameraObj = m_scene->GetObj<CameraObject>();
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

void CameraObject::OnUpdate(GameTimer& gTimer)
{
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float y = mRadius * cosf(mPhi);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);

    Object* playerObj = m_scene->GetObj<PlayerObject>();
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
    char outstatus = m_scene->ClampToBounds(pos, {0.0f, 1.0f, 0.0f});
    transform->SetPosition(pos);

    XMMATRIX transformM = transform->GetTransformM();
    XMMATRIX invtransformM = XMMatrixInverse(nullptr, transformM);
    memcpy(m_scene->GetConstantBufferMappedData(), &XMMatrixTranspose(invtransformM), sizeof(XMMATRIX)); // 처음 매개변수는 시작주소
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

void TigerObject::OnUpdate(GameTimer& gTimer)
{
    // RandomVelocity(gTimer);
    TigerBehavior(gTimer);
    Object::OnUpdate(gTimer);
}

void TigerObject::OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration)
{
    Transform* transform = GetComponent<Transform>();
    XMVECTOR pos = transform->GetPosition();
    pos += -collisionNormal * penetration;
    transform->SetPosition(pos);
}

void TigerObject::TigerBehavior(GameTimer& gTimer)
{
    Transform* transform = GetComponent<Transform>();
    XMVECTOR pos = transform->GetPosition();

    PlayerObject* player = m_scene->GetObj<PlayerObject>();
    Transform* playerTransform = player->GetComponent<Transform>();
    XMVECTOR playerPos = playerTransform->GetPosition();

    XMVECTOR vec = playerPos - pos;
    
    float speed = 15.f;
    float result = XMVectorGetX(XMVector3Length(vec));
    if (result < 200.f && result > 20.0f) {
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

void TestObject::OnProcessCollision(Object& other, XMVECTOR collisionNormal, float penetration)
{
    PlayerObject* playerObj = dynamic_cast<PlayerObject*>(&other);
    if (playerObj) {
        Delete();
    }
}
