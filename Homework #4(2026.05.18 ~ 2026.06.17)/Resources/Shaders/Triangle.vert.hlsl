struct VertexInput
{
	float3 position : POSITION;
};

struct VertexOutput
{
	float4 position : SV_Position;
	float4 color : COLOR0;
};

cbuffer CameraConstants : register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;
	float4 position = mul(float4(input.position, 1.0f), view);
	output.position = mul(position, projection);
	output.color = float4(1.0f, 0.6f, 0.2f, 1.0f);
	return output;
}
