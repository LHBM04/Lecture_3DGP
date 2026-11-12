#pragma vertex VSMain
#pragma pixel PSMain

#pragma pack_matrix(row_major)

cbuffer CameraConstants : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float3 lightDirection;
    float lightIntensity;
    float4 lightColor;
    float4 ambientLightColor;
};

cbuffer ObjectConstants : register(b1)
{
    float4x4 world;
};

cbuffer MaterialConstants : register(b2)
{
    float4 baseColor;
    float4 emissiveColor;
    float metallic;
    float roughness;
    uint textureMask;
    float materialPadding;
    float4 specularColor;
};

Texture2D<float4> baseColorTexture : register(t0);
Texture2D<float4> normalTexture : register(t1);
Texture2D<float4> metallicTexture : register(t2);
Texture2D<float4> roughnessTexture : register(t3);
Texture2D<float4> occlusionTexture : register(t4);
Texture2D<float4> emissiveTexture : register(t5);
Texture2D<float4> specularTexture : register(t6);
Texture2D<float4> maskTexture : register(t7);

SamplerState materialSampler : register(s0);

static const uint BASE_COLOR_SLOT = 0u;
static const uint NORMAL_SLOT = 1u;
static const uint METALLIC_SLOT = 2u;
static const uint ROUGHNESS_SLOT = 3u;
static const uint OCCLUSION_SLOT = 4u;
static const uint EMISSIVE_SLOT = 5u;
static const uint SPECULAR_SLOT = 6u;
static const uint MASK_SLOT = 7u;

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
    float3 viewPosition : TEXCOORD1;
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

    output.viewPosition = position.xyz;
    output.lightDirection = normalize(mul(float4(-lightDirection, 0.0f), view).xyz);
    return output;
}

bool HasTexture(uint slot)
{
    return (textureMask & (1u << slot)) != 0u;
}

float3 GetSurfaceNormal(VSOutput input)
{
    float3 normal = normalize(input.normal);
    if (!HasTexture(NORMAL_SLOT))
    {
        return normal;
    }

    float3 tangentNormal = normalTexture.Sample(materialSampler, input.uv0).xyz * 2.0f - 1.0f;
    float3 positionDx = ddx(input.viewPosition);
    float3 positionDy = ddy(input.viewPosition);

    float2 sourceUV = float2(input.uv0.x, 1.0f - input.uv0.y);
    float2 uvDx = ddx(sourceUV);
    float2 uvDy = ddy(sourceUV);
    float determinant = uvDx.x * uvDy.y - uvDx.y * uvDy.x;
    if (abs(determinant) <= 1e-10f)
    {
        return normal;
    }

    float orientation = sign(determinant);
    float3 tangent = (positionDx * uvDy.y - positionDy * uvDx.y) * orientation;
    float3 referenceBitangent = (positionDy * uvDx.x - positionDx * uvDy.x) * orientation;
    tangent -= normal * dot(normal, tangent);
    float tangentLengthSquared = dot(tangent, tangent);
    if (tangentLengthSquared <= 1e-20f)
    {
        return normal;
    }

    tangent *= rsqrt(tangentLengthSquared);
    float3 bitangent = cross(normal, tangent);
    if (dot(bitangent, referenceBitangent) < 0.0f)
    {
        bitangent = -bitangent;
    }

    return normalize(tangent * tangentNormal.x + bitangent * tangentNormal.y + normal * tangentNormal.z);
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 surfaceColor = baseColor;
    if (HasTexture(BASE_COLOR_SLOT))
    {
        surfaceColor *= baseColorTexture.Sample(materialSampler, input.uv0);
    }

    float metalness = saturate(metallic);
    float surfaceRoughness = saturate(roughness);
    float ambientOcclusion = 1.0f;
    if (HasTexture(MASK_SLOT))
    {
        float4 mask = maskTexture.Sample(materialSampler, input.uv0);
        metalness = mask.r;
        ambientOcclusion = mask.g;
        surfaceRoughness = 1.0f - mask.a;
    }
    if (HasTexture(METALLIC_SLOT))
    {
        float4 metallicGloss = metallicTexture.Sample(materialSampler, input.uv0);
        metalness = metallicGloss.r;
        surfaceRoughness = 1.0f - metallicGloss.a;
    }
    if (HasTexture(ROUGHNESS_SLOT))
    {
        surfaceRoughness = roughnessTexture.Sample(materialSampler, input.uv0).r;
    }
    if (HasTexture(OCCLUSION_SLOT))
    {
        ambientOcclusion = occlusionTexture.Sample(materialSampler, input.uv0).r;
    }

    float3 dielectricSpecular = specularColor.rgb * 0.04f;
    if (HasTexture(SPECULAR_SLOT))
    {
        dielectricSpecular = specularColor.rgb * specularTexture.Sample(materialSampler, input.uv0).rgb;
    }

    float3 emissive = emissiveColor.rgb;
    if (HasTexture(EMISSIVE_SLOT))
    {
        emissive *= emissiveTexture.Sample(materialSampler, input.uv0).rgb;
    }

    float3 normal = GetSurfaceNormal(input);
    float3 viewDirection = projection._44 == 1.0f ? float3(0.0f, 0.0f, -1.0f) : normalize(-input.viewPosition);
    float3 halfDirection = viewDirection + input.lightDirection;
    halfDirection *= rsqrt(max(dot(halfDirection, halfDirection), 1e-6f));

    float nDotL = saturate(dot(normal, input.lightDirection));
    float nDotH = saturate(dot(normal, halfDirection));
    float smoothness = 1.0f - surfaceRoughness;
    float shininess = lerp(4.0f, 128.0f, smoothness * smoothness);

    float3 diffuse = surfaceColor.rgb * (1.0f - metalness);
    float3 reflectance = lerp(dielectricSpecular, surfaceColor.rgb, metalness);
    float3 specular = reflectance * pow(nDotH, shininess);
    float3 ambient = surfaceColor.rgb * (ambientLightColor.rgb * ambientOcclusion);
    float3 directLight = (diffuse + specular) * (nDotL * lightColor.rgb * lightIntensity);
    float3 litColor = ambient + directLight + emissive;
    return float4(litColor, surfaceColor.a);
}
