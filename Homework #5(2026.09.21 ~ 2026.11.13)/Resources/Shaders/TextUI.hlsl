#pragma vertex VSMain
#pragma pixel PSMain

// @cull None
// @depth Off
// @depthWrite Off
// @blend Alpha
// @topology TriangleStrip

#pragma pack_matrix(row_major)

cbuffer ObjectConstants : register(b1)
{
    float4x4 world;
};

cbuffer TextUIConstants : register(b2)
{
    float2 viewportSize;
    float2 padding;
    float4 glyphRect;
    float4 glyphUV;
    float4 textColor;
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
    const float2 corner = float2(vertexID & 1u, vertexID >> 1u);
    output.uv = lerp(glyphUV.xy, glyphUV.zw, corner);

    const float2 localPosition = glyphRect.xy + corner * glyphRect.zw;
    const float2 pixelPosition = mul(float4(localPosition, 0.0f, 1.0f), world).xy;
    const float2 clipPosition = pixelPosition / viewportSize * float2(2.0f, -2.0f);
    output.position = float4(clipPosition, 0.0f, 1.0f);

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return imageTexture.Sample(linearClamp, input.uv) * textColor;
}
