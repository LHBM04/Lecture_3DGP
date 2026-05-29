cbuffer CameraBuffer : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

#define MAX_LIGHTS 8

struct LightData
{
    float4 color;
    float4 direction;
};

cbuffer LightBuffer : register(b1)
{
    float4 ambientColor;
    float4 cameraPosition;
    LightData lights[MAX_LIGHTS];
    uint activeLightCount;
    float3 padding;
};

#define MAX_INSTANCES 1024

cbuffer ObjectBuffer : register(b2)
{
    float4x4 worldMatrices[MAX_INSTANCES];
};

cbuffer MaterialBuffer : register(b3)
{
    float4 baseColor;
    float roughness;
    float metallic;
    float2 matPadding;
};

struct VertexInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 color : COLOR;
};

VertexOutput VSMain(VertexInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;
    
    float4x4 worldMatrix = worldMatrices[instanceID];
    
    float4 worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    output.worldPosition = worldPos;
    
    float4 viewPos = mul(worldPos, viewMatrix);
    output.position = mul(viewPos, projectionMatrix);
    
    output.color = input.color;
    return output;
}

struct PixelOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
};

PixelOutput PSMain(VertexOutput input)
{
    PixelOutput output;
    
    // Albedo G-Buffer (BaseColor * VertexColor)
    output.albedo = baseColor * input.color;
    
    // Normal G-Buffer (일단 정적 노멀 사용, 실제론 정점 Normal 필요)
    output.normal = float4(0.0f, 1.0f, 0.0f, 1.0f);
    
    return output;
}
