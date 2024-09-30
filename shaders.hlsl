cbuffer SceneConstantBuffer : register(b0)
{
    float4 velocity;
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
    
    output.position.x = input.position.x; //+ (0.5 * cos(velocity.x));
    output.position.y = input.position.y; //+ (0.5 * sin(velocity.y));
    output.position.zw = input.position.zw;
    output.uv = input.uv;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return g_texture.Sample(g_sampler, input.uv);
}
