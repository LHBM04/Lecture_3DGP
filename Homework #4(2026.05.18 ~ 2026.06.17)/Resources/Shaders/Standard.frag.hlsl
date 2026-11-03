cbuffer MaterialConstants : register(b2, space0)
{
    float4 baseColor;
    float4 emissiveColor;
    float metallic;
    float roughness;
    float2 materialPadding;
};

struct FragmentInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(FragmentInput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 lightDirection = normalize(float3(0.5f, 1.0f, -0.5f));
    float diffuse = saturate(dot(normal, lightDirection));
    float ambient = lerp(0.10f, 0.20f, saturate(roughness));
    float direct = diffuse * lerp(1.0f, 0.6f, saturate(metallic));
    return float4(baseColor.rgb * (ambient + direct) + emissiveColor.rgb, baseColor.a);
}
