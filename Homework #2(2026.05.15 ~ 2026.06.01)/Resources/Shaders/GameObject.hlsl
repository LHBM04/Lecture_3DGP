#pragma pack_matrix(row_major)

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 positionCS : SV_POSITION;
    float3 normalWS : NORMAL;
};

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
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

    output.normalWS = normalize(mul(float4(input.normal, 0.0f), World).xyz);
    return output;
}

GBufferOutput PSMain(VSOutput input)
{
    GBufferOutput output;
    const float3 n = normalize(input.normalWS);
    output.albedo = AlbedoColor;
    output.normal = float4(n * 0.5f + 0.5f, 1.0f);
    return output;
}
