#pragma vertex VSMain
#pragma pixel PSMain
#pragma pack_matrix(row_major)

cbuffer CameraConstants : register(b0, space0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer ObjectConstants : register(b1, space0)
{
    float4x4 world;
};

cbuffer MaterialConstants : register(b2, space0)
{
    float4 baseColor;
    float4 emissiveColor;
    float metallic;
    float roughness;
    float2 materialPadding;
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
    float4 position = mul(float4(input.position, 1.0f), world);
    position = mul(position, view);
    output.position = mul(position, projection);
    output.normal = mul(float4(input.normal, 0.0f), world).xyz;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 lightDirection = normalize(float3(0.5f, 1.0f, -0.5f));
    float diffuse = saturate(dot(normal, lightDirection));
    float ambient = lerp(0.10f, 0.20f, saturate(roughness));
    float direct = diffuse * lerp(1.0f, 0.6f, saturate(metallic));
    return float4(baseColor.rgb * (ambient + direct) + emissiveColor.rgb, baseColor.a);
}
