#include "Common.hlsli"

cbuffer constants : register( b0 )
{
	//float3	lightPos;
	//float	g;
	//float	g2;
	//float3	pad;
};

float4 SkyFromAtmosphere_ps( vs_output input ) : SV_Target
{
	float g = -0.990f;
	float g2 = g * g;
	float3 lightPos = float3(0.2f, 0.0f, 0.8f);
	lightPos = normalize(lightPos);
	float cos = dot(lightPos, input.dirToCam) / length(input.dirToCam);
	float4 color = calcRayleighPhase(cos) * input.rColor + 
		calcMiePhase(cos, g, g2) * input.mColor;

	return float4(1.0f, 0.0f, 0.0f, 1.0f);
	return input.rColor;
	//return float4(calcMiePhase(cos, g, g2), calcMiePhase(cos, g, g2),
	//	calcMiePhase(cos, g, g2), 1.0f);
	//return float4(abs(color.x), abs(color.y), abs(color.z), 1.0f);
}