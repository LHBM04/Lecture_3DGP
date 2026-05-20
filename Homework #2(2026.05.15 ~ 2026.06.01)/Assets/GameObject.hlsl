// Constant Buffer: 3D 변환 행렬 (정점 위치 계산용)
cbuffer TransformBuffer : register(b0)
{
    matrix g_WorldViewProj;
};

// 정점 셰이더 입력 구조체 (Input Layout과 일치해야 함)
struct VS_INPUT
{
    float3 position : POSITION;
    float4 color : COLOR;
};

// 정점 셰이더 출력 / 픽셀 셰이더 입력 구조체
struct VS_OUTPUT
{
    float4 position : SV_POSITION; // 시스템 값: 래스터라이저가 인식하는 클립 공간 좌표
    float4 color : COLOR; // 픽셀 셰이더로 보낼 보간된 색상
};

// [정점 셰이더]
VS_OUTPUT VS_Main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // 로컬 공간의 정점을 스크린 공간(클립 공간)으로 변환
    output.position = mul(float4(input.position, 1.0f), g_WorldViewProj);
    
    // 입력받은 정점 색상을 그대로 픽셀 셰이더로 전달 (래스터라이저 단계에서 자동 보간됨)
    output.color = input.color;
    
    return output;
}

// [픽셀 셰이더]
float4 PS_Main(VS_OUTPUT input) : SV_TARGET
{
    // 최종 픽셀 색상 출력 (렌더 타겟에 기록됨)
    return input.color;
}