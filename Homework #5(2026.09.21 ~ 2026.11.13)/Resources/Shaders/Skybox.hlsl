#pragma vertex VSMain
#pragma pixel PSMain

// @cull None
// @blend Alpha
// @depth LessEqual
// @depthWrite Off

#pragma pack_matrix(row_major)

cbuffer CameraConstants : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer SkyboxConstants : register(b2)
{
    float4 color;
};

TextureCube<float4> skyboxTexture : register(t0);
SamplerState linearClamp : register(s1);

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 direction : TEXCOORD0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    const float2 ndc = float2((vertexID == 1u) ? 3.0f : -1.0f, (vertexID == 2u) ? -3.0f : 1.0f);
    float3 viewDirection = float3(0.0f, 0.0f, 1.0f);
    if (projection[2][3] != 0.0f)
    {
        viewDirection.xy = ndc / float2(projection[0][0], projection[1][1]);
    }

    VSOutput output;
    output.position = float4(ndc, 1.0f, 1.0f);
    output.direction = mul(viewDirection, transpose((float3x3)view));
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(skyboxTexture.Sample(linearClamp, input.direction).rgb, 1.0f) * color;
}
