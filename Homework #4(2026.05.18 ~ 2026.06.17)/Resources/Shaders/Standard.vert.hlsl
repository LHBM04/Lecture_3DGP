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

VSOutput main(VSInput input)
{
    VSOutput output;
    float4 position = mul(float4(input.position, 1.0f), world);
    position = mul(position, view);
    output.position = mul(position, projection);
    output.normal = mul(float4(input.normal, 0.0f), world).xyz;
    return output;
}
