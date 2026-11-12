#pragma vertex VSMain
#pragma pixel PSMain

// @cull None
// @fill Wireframe
// @depth Off
// @depthWrite Off
// @blend Opaque
// @topology TriangleList

#pragma pack_matrix(row_major)

cbuffer CameraConstants : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer ObjectConstants : register(b1)
{
    float4x4 world;
};

cbuffer DebugColorConstants : register(b2)
{
    float4 color;
};

struct VSInput
{
    float3 position : POSITION;
};

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), world), view), projection);
    return output;
}

float4 PSMain() : SV_TARGET
{
    return color;
}
