#pragma vertex VSMain
#pragma pixel PSMain

// @cull None
// @depth Off
// @depthWrite Off
// @blend Alpha
// @topology LineStrip

#pragma pack_matrix(row_major)

cbuffer ObjectConstants : register(b1)
{
    float4x4 world;
};

cbuffer ImageUIConstants : register(b2)
{
    float2 viewportSize;
    float2 padding;
    float4 color;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    static const float2 CORNERS[5] =
    {
        float2(-0.5f, -0.5f),
        float2( 0.5f, -0.5f),
        float2( 0.5f,  0.5f),
        float2(-0.5f,  0.5f),
        float2(-0.5f, -0.5f)
    };

    VSOutput output;
    const float2 localPosition = CORNERS[vertexID % 5u];
    const float2 pixelPosition = mul(float4(localPosition, 0.0f, 1.0f), world).xy;
    const float2 clipPosition = pixelPosition / viewportSize * float2(2.0f, -2.0f);
    output.position = float4(clipPosition, 0.0f, 1.0f);
    return output;
}

float4 PSMain() : SV_TARGET
{
    return color;
}
