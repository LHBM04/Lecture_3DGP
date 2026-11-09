#pragma vertex VSMain
#pragma pixel PSMain

#pragma pack_matrix(row_major)

cbuffer CameraBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
};

TextureCube<float4> skyboxTexture : register(t0);
SamplerState linearClamp : register(s1);

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 direction : TEXCOORD0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    // 화면을 덮는 삼각형의 클립 좌표를 생성합니다.
    const float2 ndc = float2((vertexID == 1u) ? 3.0f : -1.0f, (vertexID == 2u) ? -3.0f : 1.0f);
    float3 viewDirection = float3(0.0f, 0.0f, 1.0f);
    if (projectionMatrix[2][3] != 0.0f)
    {
        // 원근 투영에서 NDC 좌표를 카메라 공간의 광선 방향으로 되돌립니다.
        viewDirection.xy = ndc / float2(projectionMatrix[0][0], projectionMatrix[1][1]);
    }

    VSOutput output;
    output.position = float4(ndc, 1.0f, 1.0f);
    // 뷰 행렬의 회전만 역으로 적용하므로 카메라 이동은 배경에 영향을 주지 않습니다.
    output.direction = mul(viewDirection, transpose((float3x3)viewMatrix));
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    return float4(skyboxTexture.Sample(linearClamp, input.direction).rgb, 1.0f);
}
