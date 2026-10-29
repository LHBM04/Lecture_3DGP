cbuffer CameraConstants : register(b0)
{
	row_major float4x4 view;
	row_major float4x4 projection;
};

struct VertexInput
{
	float3 position : POSITION;
	float4 color : COLOR0;
};

struct VertexOutput
{
	float4 position : SV_Position;
	float4 color : COLOR0;
};

VertexOutput VSMain(VertexInput input)
{
	VertexOutput output;
	float4 position = mul(float4(input.position, 1.0f), view);
	output.position = mul(position, projection);
	output.color = input.color;
	return output;
}

float4 PSMain(VertexOutput input) : SV_Target0
{
	return input.color;
}
