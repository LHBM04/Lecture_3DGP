#pragma vertex VSMain
#pragma pixel PSMain

// @cull None
// @depth Off
// @depthWrite Off
// @blend Opaque
// @topology LineList

#pragma pack_matrix(row_major)

cbuffer CameraConstants : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer DebugLineConstants : register(b2)
{
    float4 color;
    float4 positions[192];
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    output.position = mul(mul(positions[vertexID], view), projection);
    return output;
}

float4 PSMain() : SV_TARGET
{
    return color;
}
