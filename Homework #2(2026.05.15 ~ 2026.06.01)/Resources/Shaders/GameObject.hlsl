#pragma pack_matrix(row_major)

struct VSInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float3 normalWS : NORMAL;
};

// Slot 0: Camera Constants
cbuffer CameraConstants : register(b0)
{
	matrix View;
	matrix Projection;
};

// Slot 1: Light Constants
cbuffer LightConstants : register(b1)
{
	float4 AmbientColor;
	float4 Lights[8]; // color(rgb), intensity(a)
	float4 LightDirs[8]; // direction(xyz), active(a)
	float3 CameraPosition;
	uint ActiveLightCount;
};

// Slot 2: Object Constants
cbuffer ObjectConstants : register(b2)
{
	matrix World;
};

// Slot 4: Material Constants
cbuffer MaterialConstants : register(b3)
{
	float4 AlbedoColor;
	float Roughness;
	float Metallic;
	float2 Padding;
};

PSInput VSMain(VSInput input)
{
	PSInput output;
	output.position = mul(float4(input.position, 1.0f), World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.normalWS = normalize(mul(float4(input.normal, 0.0f), World).xyz);
	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float3 n = normalize(input.normalWS);
	float3 totalDiffuse = AmbientColor.rgb;
	
	for(uint i = 0; i < ActiveLightCount; ++i)
	{
		float3 l = normalize(-LightDirs[i].xyz);
		float ndotl = saturate(dot(n, l));
		totalDiffuse += Lights[i].rgb * Lights[i].a * ndotl;
	}

	float3 litColor = AlbedoColor.rgb * totalDiffuse;
	return float4(litColor, AlbedoColor.a);
}
