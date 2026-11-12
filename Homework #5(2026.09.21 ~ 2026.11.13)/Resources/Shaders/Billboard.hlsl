#pragma vertex VSMain
#pragma pixel PSMain

// @cull None
// @depth LessEqual
// @depthWrite Off
// @blend Alpha
// @topology TriangleStrip

#pragma pack_matrix(row_major)

cbuffer CameraConstants : register(b0)
{
    float4x4 view;
    float4x4 projection;
};

cbuffer ObjectConstants : register(b1)
{
    float4x4 world;
};

cbuffer BillboardConstants : register(b2)
{
    float2 billboardSize;
    uint frameIndex;
    uint columns;
    uint rows;
    float3 padding;
    float4 billboardColor;
};

Texture2D<float4> billboardTexture : register(t0);
SamplerState linearClamp : register(s1);

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;

    const float2 quadUV = float2(vertexID & 1u, vertexID >> 1u);
    const float2 localOffset = float2(quadUV.x - 0.5f, 0.5f - quadUV.y) * billboardSize;

    const uint validColumns = max(1u, columns);
    const uint validRows = max(1u, rows);
    const uint col = frameIndex % validColumns;
    const uint row = frameIndex / validColumns;
    const float2 frameSize = float2(1.0f / float(validColumns), 1.0f / float(validRows));
    output.uv = float2(col, row) * frameSize + quadUV * frameSize;

    const float3 worldPosition = world[3].xyz;
    float4 viewPosition = mul(float4(worldPosition, 1.0f), view);
    viewPosition.xy += localOffset;

    output.position = mul(viewPosition, projection);
    output.color = billboardColor;

    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float4 texColor = billboardTexture.Sample(linearClamp, input.uv);
    float4 finalColor = texColor * input.color;
    clip(finalColor.a - 0.01f);

    return finalColor;
}
