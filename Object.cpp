#include "Object.h"
#include "GameTimer.h"

Object::Object(std::wstring name) :
    m_name(name)
{
}

std::wstring Object::GetObjectName()
{
    return m_name;
}

void PlayerObject::OnUpdate(GameTimer& gTimer)
{
    shared_ptr<Rotation> rotationCom = nullptr;
    shared_ptr<Rotate> rotateCom = nullptr;
    XMMATRIX rotate = XMMatrixIdentity();
    if (GetComponent(&rotationCom) && GetComponent(&rotateCom) && rotationCom->GetModify()) {
        rotationCom->SetRotation(rotationCom->GetRotation() + rotateCom->GetRotate() * gTimer.DeltaTime());
        rotate = XMMatrixRotationRollPitchYawFromVector(rotationCom->GetRotation() * (XM_PI / 180.0f));
    }

    XMMATRIX translate = OnKeyboardInput(gTimer);

    // 결과를 오브젝트의 컴포넌트에 저장
    shared_ptr<WorldMatrix> matrixData = nullptr;
    if (GetComponent(&matrixData) && rotationCom->GetModify()) {
        // 월드 행렬 = 회전 행렬 * 이동 행렬
        XMMATRIX world = rotate * translate;
        matrixData->SetMatrix(world);
        //rotationCom->SetModify(false);
        //positionCom->SetModify(false);
    }
}

void PlayerObject::OnRender(ID3D12GraphicsCommandList* commandList)
{
    shared_ptr<WorldMatrix> matrixData = nullptr;
    XMMATRIX world = XMMatrixIdentity();
    if (GetComponent(&matrixData)) {
        world = matrixData->GetMatrix();
    }
    commandList->SetGraphicsRoot32BitConstants(2, 16, &XMMatrixTranspose(world), 0);
    shared_ptr<Mesh> meshData = nullptr;
    if (GetComponent(&meshData)) {
        SubMeshData tmp = meshData->GetSubMeshData();
        commandList->DrawIndexedInstanced(tmp.indexCountPerInstance, 1, tmp.statIndexLocation, tmp.baseVertexLocation, 0);
    }

}

XMMATRIX PlayerObject::OnKeyboardInput(const GameTimer& gTimer)
{

    shared_ptr<Position> positionCom = nullptr;
    shared_ptr<Velocity> velocityCom = nullptr;
    GetComponent(&positionCom);
    GetComponent(&velocityCom);
    //positionCom->GetModify();
    float speed = 3;

    const float dt = gTimer.DeltaTime();

    if (GetAsyncKeyState('W') & 0x8000) {
        velocityCom->SetVelocity(XMVECTOR({ 0,0,speed,0 }));
        positionCom->SetPosition(positionCom->GetPosition() + velocityCom->GetVelocity() * gTimer.DeltaTime());
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        velocityCom->SetVelocity(XMVECTOR({ 0,0,-speed,0 }));
        positionCom->SetPosition(positionCom->GetPosition() + velocityCom->GetVelocity() * gTimer.DeltaTime());
    }

    if (GetAsyncKeyState('A') & 0x8000) {
        velocityCom->SetVelocity(XMVECTOR({ -speed,0,0,0 }));
        positionCom->SetPosition(positionCom->GetPosition() + velocityCom->GetVelocity() * gTimer.DeltaTime());
    }

    if (GetAsyncKeyState('D') & 0x8000) {
        velocityCom->SetVelocity(XMVECTOR({ speed,0,0,0 }));
        positionCom->SetPosition(positionCom->GetPosition() + velocityCom->GetVelocity() * gTimer.DeltaTime());
    }

    XMMATRIX translate = XMMatrixIdentity();
    translate = XMMatrixTranslationFromVector(positionCom->GetPosition());
    return translate;
}

void SampleObject::OnUpdate(GameTimer& gTimer)
{
    shared_ptr<Rotation> rotationCom = nullptr;
    shared_ptr<Rotate> rotateCom = nullptr;
    XMMATRIX rotate = XMMatrixIdentity();
    if (GetComponent(&rotationCom) && GetComponent(&rotateCom) && rotationCom->GetModify()) {
        rotationCom->SetRotation(rotationCom->GetRotation() + rotateCom->GetRotate() * gTimer.DeltaTime());
        rotate = XMMatrixRotationRollPitchYawFromVector(rotationCom->GetRotation() * (XM_PI / 180.0f));
    }
    shared_ptr<Position> positionCom = nullptr;
    shared_ptr<Velocity> velocityCom = nullptr;
    XMMATRIX translate = XMMatrixIdentity();
    if (GetComponent(&positionCom) && GetComponent(&velocityCom) && positionCom->GetModify()) {
        positionCom->SetPosition(positionCom->GetPosition() + velocityCom->GetVelocity() * gTimer.DeltaTime());
        translate = XMMatrixTranslationFromVector(positionCom->GetPosition());
    }

    // 결과를 오브젝트의 컴포넌트에 저장
    shared_ptr<WorldMatrix> matrixData = nullptr;
    if (GetComponent(&matrixData)) {
        // 월드 행렬 = 회전 행렬 * 이동 행렬
        XMMATRIX world = rotate * translate;
        matrixData->SetMatrix(world);
    }
}

void SampleObject::OnRender(ID3D12GraphicsCommandList* commandList)
{
    shared_ptr<WorldMatrix> matrixData = nullptr;
    XMMATRIX world = XMMatrixIdentity();
    if (GetComponent(&matrixData)) {
        world = matrixData->GetMatrix();
    }
    commandList->SetGraphicsRoot32BitConstants(2, 16, &XMMatrixTranspose(world), 0);
    shared_ptr<Mesh> meshData = nullptr;
    if (GetComponent(&meshData)) {
        SubMeshData tmp = meshData->GetSubMeshData();
        commandList->DrawIndexedInstanced(tmp.indexCountPerInstance, 1, tmp.statIndexLocation, tmp.baseVertexLocation, 0);
    }

}
