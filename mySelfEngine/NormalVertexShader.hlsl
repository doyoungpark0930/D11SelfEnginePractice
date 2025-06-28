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
    float3 Pos: POSITION;
    float3 Normal : NORMAL;
    float3 StartPos : COLOR;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
    VS_OUTPUT output;
    
    float size = length(input.Pos - input.StartPos) * 0.4f;
    float3 NormalVector = normalize(mul(float4(input.Normal, 0.0f), NormalModel).xyz);
    float3 startPos = mul(float4(input.StartPos, 1.0f), Model).xyz;
    float3 endPos = startPos + NormalVector * size;
    
    
    output.Pos = mul(float4(endPos, 1.0f), ViewProj);

    return output;
}
