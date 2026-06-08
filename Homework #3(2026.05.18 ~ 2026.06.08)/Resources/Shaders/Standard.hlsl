#pragma pack_matrix(row_major)

cbuffer cbCameraInfo : register(b1)
{
    float4x4 gViewMatrix;
    float4x4 gProjectionMatrix;
    float4x4 gViewProjectionMatrix;
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
    float4 instanceWorld0 : INSTANCE_WORLD0;
    float4 instanceWorld1 : INSTANCE_WORLD1;
    float4 instanceWorld2 : INSTANCE_WORLD2;
    float4 instanceWorld3 : INSTANCE_WORLD3;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    const float4x4 worldMatrix = float4x4(
        input.instanceWorld0,
        input.instanceWorld1,
        input.instanceWorld2,
        input.instanceWorld3);

    const float4 worldPosition = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(worldPosition, gViewProjectionMatrix);
    output.normal = normalize(mul(float4(input.normal, 0.0f), worldMatrix).xyz);

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    const float3 lightDirection = normalize(-gLightDirection.xyz);
    const float diffuse = saturate(dot(normalize(input.normal), lightDirection));
    const float shapedDiffuse = pow(diffuse, 1.15f);
    const float roughAmbient = lerp(0.03f, 0.10f, saturate(gRoughness));
    const float metallicAttenuation = lerp(1.0f, 0.6f, saturate(gMetallic));
    const float3 ambient = gBaseColor.rgb * roughAmbient;
    const float3 direct = gBaseColor.rgb * gLightColor.rgb * shapedDiffuse * 1.35f * metallicAttenuation;
    const float3 lit = ambient + direct + gEmissiveColor.rgb;

    return float4(lit, gBaseColor.a);
}
