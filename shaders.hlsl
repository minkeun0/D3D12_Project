cbuffer WoldTranslate : register(b0)
{
    float4x4 world;
};

cbuffer SceneConstantBuffer : register(b1)
{
    float4x4 viewProj;
};

Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

struct VSInput
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{    
    VSOutput output;
    output.position = mul(input.position, mul(world, viewProj));
    float3 n = mul(input.normal.xyz, (float3x3) world); // fbx 파일에서 읽어올때 회전을 시켜서 읽어 오는게 더 좋을지도...
    output.normal = float4(n, 0);
    output.uv = input.uv;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 lightVector = float4(0, 1, 0, 0);
    //float4 result = float4(1, 1, 1, 0) * dot((input.normal), normalize(eye - at));
    float4 result = Texture.Sample(Sampler, input.uv) * max(dot(normalize(input.normal), lightVector),0.2);
    return result;
}
