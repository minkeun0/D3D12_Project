cbuffer SceneConstantBuffer : register(b0)
{
    float4 velocity;
};

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    
    output.position.x = input.position.x + (0.5 * cos(velocity.x));
    output.position.y = input.position.y + (0.5 * sin(velocity.y));
    output.position.zw = input.position.zw;
    output.color = input.color;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
