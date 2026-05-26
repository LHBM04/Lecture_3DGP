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
	float3 normalWS : NORMAL;
};

cbuffer CameraConstants : register(b0)
{
	matrix View;
	matrix Projection;
};

cbuffer ObjectConstants : register(b1)
{
	matrix World;
	matrix InverseWorld;
};

cbuffer MaterialConstants : register(b2)
{
	float4 AlbedoColor;
};

cbuffer LightConstants : register(b3)
{
	float3 LightDirection;
	float LightIntensity;

	float3 LightColor;
	float LightPadding;
};

PSInput VSMain(VSInput input)
{
	PSInput output;
	matrix instanceWorld = matrix(input.world0, input.world1, input.world2, input.world3);
	output.position = mul(float4(input.position, 1.0f), instanceWorld);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.normalWS = normalize(mul(float4(input.normal, 0.0f), instanceWorld).xyz);
	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float3 n = normalize(input.normalWS);
	float3 l = normalize(-LightDirection);
	float ndotl = saturate(dot(n, l));
	float diffuse = max(0.1f, ndotl * LightIntensity);
	float3 litColor = AlbedoColor.rgb * LightColor * diffuse;
	return float4(litColor, AlbedoColor.a);
}
