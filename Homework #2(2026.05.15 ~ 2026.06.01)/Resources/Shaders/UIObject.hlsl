#pragma pack_matrix(row_major)

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 positionCS : SV_POSITION;
};

cbuffer CameraConstants : register(b0)
{
    matrix View;
    matrix Projection;
};

cbuffer ObjectConstants : register(b2)
{
    matrix World;
};

cbuffer MaterialConstants : register(b3)
{
    float4 AlbedoColor;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    const float4 positionWS = mul(float4(input.position, 1.0f), World);
    output.positionCS = mul(positionWS, View);
    output.positionCS = mul(output.positionCS, Projection);

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return AlbedoColor;
}
