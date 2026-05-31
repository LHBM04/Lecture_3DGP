#pragma pack_matrix(row_major)

struct VSOutput
{
    float4 positionCS : SV_POSITION;
    float2 uv : TEXCOORD0;
};

cbuffer CameraConstants : register(b0)
{
    matrix View;
    matrix Projection;
};

cbuffer LightConstants : register(b1)
{
    float4 AmbientColor;
    float4 Lights[8];
    float4 LightDirs[8];
    float3 CameraPosition;
    uint ActiveLightCount;
};

Texture2D AlbedoTex : register(t0);
Texture2D NormalTex : register(t1);
SamplerState LinearSampler : register(s0);

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    float2 pos;
    pos.x = (vertexID == 2) ? 3.0f : -1.0f;
    pos.y = (vertexID == 1) ? -3.0f : 1.0f;

    output.positionCS = float4(pos, 0.0f, 1.0f);
    output.uv = float2((pos.x + 1.0f) * 0.5f, 1.0f - (pos.y + 1.0f) * 0.5f);
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    const float4 albedo = AlbedoTex.Sample(LinearSampler, input.uv);
    const float3 n = normalize(NormalTex.Sample(LinearSampler, input.uv).xyz * 2.0f - 1.0f);

    float3 totalDiffuse = AmbientColor.rgb;
    [loop]
    for (uint i = 0; i < ActiveLightCount; ++i)
    {
        const float3 l = normalize(-LightDirs[i].xyz);
        totalDiffuse += Lights[i].rgb * Lights[i].a * saturate(dot(n, l));
    }

    return float4(albedo.rgb * totalDiffuse, albedo.a);
}

