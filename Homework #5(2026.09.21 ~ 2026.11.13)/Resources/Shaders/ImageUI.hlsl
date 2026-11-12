#pragma vertex VSMain
#pragma pixel PSMain

#pragma pack_matrix(row_major)

cbuffer ObjectConstants : register(b1)
{
    float4x4 world;
};

cbuffer ImageUIConstants : register(b2)
{
    float2 viewportSize;
};

Texture2D<float4> imageTexture : register(t0);
SamplerState linearClamp : register(s1);

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    output.uv = float2(vertexID & 1u, vertexID >> 1u);

    const float2 localPosition = output.uv - float2(0.5f, 0.5f);
    const float2 pixelPosition = mul(float4(localPosition, 0.0f, 1.0f), world).xy;
    const float2 clipPosition = pixelPosition / viewportSize * float2(2.0f, -2.0f);
    output.position = float4(clipPosition, 0.0f, 1.0f);

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return imageTexture.Sample(linearClamp, input.uv);
}
