Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

float4 PSMain(VS_OUTPUT input) : SV_TARGET
{
    float4 Color = g_Texture.Sample(g_Sampler, input.Tex);
    return Color;
}
