#pragma vertex VSMain
#pragma pixel PSMain
#pragma pack_matrix(row_major)

// 카메라 상수
cbuffer CameraConstants : register(b0, space0)
{
    float4x4 view;
    float4x4 projection;
};

// 오브젝트 상수
cbuffer ObjectConstants : register(b1, space0)
{
    float4x4 world;
};

// 머티리얼 상수
cbuffer MaterialConstants : register(b2, space0)
{
    float4 baseColor;
    float4 emissiveColor;
    float metallic;
    float roughness;
    uint hasBaseColorTexture;
    float materialPadding;
};

// 베이스 컬러 텍스처
Texture2D<float4> baseColorTexture : register(t0, space0);

// 베이스 컬러 정적 샘플러
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
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 position = mul(float4(input.position, 1.0f), world);
    position = mul(position, view);
    output.position = mul(position, projection);
    output.normal = mul(float4(input.normal, 0.0f), world).xyz;
    output.uv0 = input.uv0;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 normal = normalize(input.normal);
    float3 lightDirection = normalize(float3(0.5f, 1.0f, -0.5f));
    float diffuse = saturate(dot(normal, lightDirection));
    float ambient = lerp(0.10f, 0.20f, saturate(roughness));
    float direct = diffuse * lerp(1.0f, 0.6f, saturate(metallic));
    float4 surfaceColor = baseColor;
    if (hasBaseColorTexture != 0)
    {
        surfaceColor *= baseColorTexture.Sample(baseColorSampler, input.uv0);
    }

    return float4(surfaceColor.rgb * (ambient + direct) + emissiveColor.rgb, surfaceColor.a);
}
