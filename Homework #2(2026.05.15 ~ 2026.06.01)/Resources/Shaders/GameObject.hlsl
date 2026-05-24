#pragma pack_matrix(row_major)

struct VSInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 world0 : INSTANCEWORLD0;
	float4 world1 : INSTANCEWORLD1;
	float4 world2 : INSTANCEWORLD2;
	float4 world3 : INSTANCEWORLD3;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

cbuffer CameraConstants : register(b0)
{
	matrix View;
	matrix Projection;
};

cbuffer ObjectConstants : register(b1)
{
	matrix World;
};

cbuffer MaterialConstants : register(b2)
{
	float4 AlbedoColor;
};

PSInput VSMain(VSInput input)
{
	PSInput output;
	matrix instanceWorld = matrix(input.world0, input.world1, input.world2, input.world3);
	output.position = mul(float4(input.position, 1.0f), instanceWorld);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.color = abs(input.normal);
	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return AlbedoColor;
}
