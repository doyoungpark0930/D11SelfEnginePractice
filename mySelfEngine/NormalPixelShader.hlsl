Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float4 Color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return Color;
}
