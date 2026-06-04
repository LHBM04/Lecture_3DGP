#pragma pack_matrix(row_major)

cbuffer CameraConstants : register(b0)
{
    float4x4 View;
    float4x4 Projection;
};

cbuffer LightConstants : register(b1)
{
    float4 AmbientColor;
    float4 LightColor;
    float4 LightDirection;
};

cbuffer ObjectConstants : register(b2)
{
    float4x4 World;
};

cbuffer MaterialConstants : register(b3)
{
    float4 AlbedoColor;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 positionCS : SV_POSITION;
    float3 normalWS : TEXCOORD0;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    const float4 worldPosition = mul(float4(input.position, 1.0f), World);
    const float4 viewPosition = mul(worldPosition, View);
    output.positionCS = mul(viewPosition, Projection);
    output.normalWS = normalize(mul(float4(input.normal, 0.0f), World).xyz);
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    const float3 normalWS = normalize(input.normalWS);
    const float3 lightDir = normalize(-LightDirection.xyz);
    const float NdotL = saturate(dot(normalWS, lightDir));
    const float3 lighting = AmbientColor.rgb + LightColor.rgb * NdotL;
    return float4(AlbedoColor.rgb * lighting, AlbedoColor.a);
}
