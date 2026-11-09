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
    uint hasBaseColorTexture;
    float materialPadding;
};

Texture2D<float4> baseColorTexture : register(t0, space0);

SamplerState baseColorSampler : register(s0, space0);

struct VSInput
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv0 : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv0 : TEXCOORD0;
    float3 viewDirection : TEXCOORD1;
    nointerpolation float3 lightDirection : TEXCOORD2;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 position = mul(float4(input.position, 1.0f), world);
    position = mul(position, view);
    output.position = mul(position, projection);
    float3 worldNormal = mul(float4(input.normal, 0.0f), world).xyz;
    output.normal = mul(float4(worldNormal, 0.0f), view).xyz;
    output.uv0 = input.uv0;

    // Orthographic rays are parallel along +Z in this left-handed view space.
    output.viewDirection = projection._44 == 1.0f ? float3(0.0f, 0.0f, -1.0f) : -position.xyz;
    output.lightDirection = normalize(mul(float4(0.5f, 1.0f, -0.5f, 0.0f), view).xyz);
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 surfaceColor = baseColor;
    if (hasBaseColorTexture != 0)
    {
        surfaceColor *= baseColorTexture.Sample(baseColorSampler, input.uv0);
    }

    float3 normal = normalize(input.normal);
    float3 viewDirection = normalize(input.viewDirection);
    float3 halfDirection = viewDirection + input.lightDirection;
    halfDirection *= rsqrt(max(dot(halfDirection, halfDirection), 1e-6f));

    float nDotL = saturate(dot(normal, input.lightDirection));
    float nDotH = saturate(dot(normal, halfDirection));
    float metalness = saturate(metallic);
    float smoothness = 1.0f - saturate(roughness);
    float shininess = lerp(4.0f, 128.0f, smoothness * smoothness);

    float3 diffuse = surfaceColor.rgb * (1.0f - metalness);
    float3 specularColor = lerp(float3(0.04f, 0.04f, 0.04f), surfaceColor.rgb, metalness);
    float3 specular = specularColor * pow(nDotH, shininess);
    float3 ambient = surfaceColor.rgb * 0.15f;
    float3 litColor = ambient + (diffuse + specular) * nDotL + emissiveColor.rgb;
    return float4(litColor, surfaceColor.a);
}
