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
    GetComponent<World>().SetXMMATRIX(scale * rotate * translate);
};

void PlayerObject::OnRender(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRoot32BitConstants(2, 16, &XMMatrixTranspose(GetComponent<World>().GetXMMATRIX()), 0);    
    //commandList->DrawIndexedInstanced(tmp.indexCountPerInstance, 1, tmp.statIndexLocation, tmp.baseVertexLocation, 0);
    commandList->DrawInstanced(GetComponent<Mesh>().mSubMeshData.vertexCountPerInstance, 1, GetComponent<Mesh>().mSubMeshData.startVertexLocation, 0);
}

void PlayerObject::OnKeyboardInput(const GameTimer& gTimer)
{
    float speed = 3;

    if (GetAsyncKeyState('W') & 0x8000) {
        GetComponent<Velocity>().SetXMVECTOR(XMVECTOR{ 0, 0, speed, 0 });
        GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        GetComponent<Velocity>().SetXMVECTOR(XMVECTOR{ 0, 0, -speed, 0 });
        GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    }

    if (GetAsyncKeyState('A') & 0x8000) {
        GetComponent<Velocity>().SetXMVECTOR(XMVECTOR{ -speed, 0, 0, 0 });
        GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
    }

    if (GetAsyncKeyState('D') & 0x8000) {
        GetComponent<Velocity>().SetXMVECTOR(XMVECTOR{ speed, 0, 0, 0 });
        GetComponent<Position>().SetXMVECTOR(GetComponent<Position>().GetXMVECTOR() + GetComponent<Velocity>().GetXMVECTOR() * gTimer.DeltaTime());
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
}

void TestObject::OnRender(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetGraphicsRoot32BitConstants(2, 16, &XMMatrixTranspose(GetComponent<World>().GetXMMATRIX()), 0);
    //commandList->DrawIndexedInstanced(tmp.indexCountPerInstance, 1, tmp.statIndexLocation, tmp.baseVertexLocation, 0);
    commandList->DrawInstanced(GetComponent<Mesh>().mSubMeshData.vertexCountPerInstance, 1, GetComponent<Mesh>().mSubMeshData.startVertexLocation, 0);
}
