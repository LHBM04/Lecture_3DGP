#pragma pack_matrix(row_major)

cbuffer cbCameraInfo : register(b1)
{
    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
    float4x4 gViewProjectionMatrix;
};

cbuffer cbGameObjectInfo : register(b2)
{
    float4x4 gWorldMatrix;
};

cbuffer cbMaterialInfo : register(b3)
{
    float4 gBaseColor;
    float4 gEmissiveColor;
    float gMetallic;
    float gRoughness;
    float2 gMaterialPadding;
};

cbuffer cbLightInfo : register(b4)
{
    float4 gLightDirection;
    float4 gLightColor;
};

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;

    const float4 worldPosition = mul(float4(input.position, 1.0f), gWorldMatrix);
    output.position = mul(worldPosition, gViewProjectionMatrix);
    output.normal = normalize(mul(float4(input.normal, 0.0f), gWorldMatrix).xyz);

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    const float3 lightDirection = normalize(-gLightDirection.xyz);
    const float diffuse = saturate(dot(normalize(input.normal), lightDirection));
    const float roughAmbient = lerp(0.08f, 0.24f, saturate(gRoughness));
    const float metallicAttenuation = lerp(1.0f, 0.6f, saturate(gMetallic));
    const float3 ambient = gBaseColor.rgb * roughAmbient;
    const float3 direct = gBaseColor.rgb * gLightColor.rgb * diffuse * metallicAttenuation;
    const float3 lit = ambient + direct + gEmissiveColor.rgb;

    return float4(lit, gBaseColor.a);
}
