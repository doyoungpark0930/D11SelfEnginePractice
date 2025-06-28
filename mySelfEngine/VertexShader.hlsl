cbuffer VIEWPROJ_CONSTANT : register(b0)
{
    float4x4 ViewProj;
};

cbuffer MODEL_CONSTANT : register(b1)
{
    float4x4 Model;
    float4x4 NormalModel; 
};


struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float4 worldPos = mul(float4(input.Pos, 1.0f), Model);
    
    output.Pos = mul(worldPos, ViewProj);
    output.Normal = normalize(mul(float4(input.Normal, 0.0f), NormalModel).xyz);
    output.Tex = input.Tex; 
    return output;
}
