#pragma vertex VSMain
#pragma pixel PSMain

// @cull None
// @depth Off
// @depthWrite Off
// @blend Alpha

#pragma pack_matrix(row_major)

cbuffer FadeConstants : register(b3)
{
    float4 fadeColor;
};

float4 VSMain(uint vertexID : SV_VertexID) : SV_POSITION
{
    const float2 position = float2(
        vertexID == 1u ? 3.0f : -1.0f,
        vertexID == 2u ? -3.0f : 1.0f);
    return float4(position, 0.0f, 1.0f);
}

float4 PSMain() : SV_TARGET
{
    return fadeColor;
}
