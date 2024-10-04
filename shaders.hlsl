cbuffer WoldTranslate : register(b0)
{
    float4x4 world;
};

cbuffer SceneConstantBuffer : register(b1)
{
    float4x4 viewProj;
};


Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    
    float4 tmp = mul(input.position, mul(world, viewProj));
    
    output.position = tmp;
    output.uv = input.uv;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv);
}
